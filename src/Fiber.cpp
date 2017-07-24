#include <fjs/Fiber.h>
#include <fjs/Common.h>
#ifdef _WIN32
#include <Windows.h>
#endif

// TODO: Add exceptiosn for invalid stuff?

static void LaunchFiber(fjs::Fiber* fiber)
{
	auto callback = fiber->GetCallback();
	if (callback == nullptr)
		throw fjs::Exception("LaunchFiber: callback is nullptr");

	callback(fiber);
}

fjs::Fiber::Fiber()
{
#ifdef _WIN32
	m_fiber = CreateFiber(0, (LPFIBER_START_ROUTINE)LaunchFiber, this);
	m_thread_fiber = false;
#endif
}

fjs::Fiber::~Fiber()
{
#ifdef _WIN32
	if (m_fiber && !m_thread_fiber)
		DeleteFiber(m_fiber);
#endif
}

void fjs::Fiber::FromCurrentThread()
{
#ifdef _WIN32
	if (m_fiber && !m_thread_fiber)
		DeleteFiber(m_fiber);

	m_fiber = ConvertThreadToFiber(nullptr);
	m_thread_fiber = true;
#endif
}

void fjs::Fiber::SetCallback(Callback_t cb)
{
	if (cb == nullptr)
		throw fjs::Exception("callback cannot be nullptr");

	m_callback = cb;
}

void fjs::Fiber::SwitchTo(fjs::Fiber* fiber, void* userdata)
{
	if (fiber == nullptr || fiber->m_fiber == nullptr)
		throw fjs::Exception("Invalid fiber (nullptr or invalid)");

	fiber->m_userdata = userdata;
	fiber->m_return_fiber = this;

	SwitchToFiber(fiber->m_fiber);
}

void fjs::Fiber::SwitchBack()
{
	if (m_return_fiber && m_return_fiber->m_fiber)
		SwitchToFiber(m_return_fiber->m_fiber);
	else
		throw fjs::Exception("Unable to switch back from Fiber (none or invalid return fiber)");
}