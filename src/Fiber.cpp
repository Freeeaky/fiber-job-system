#include <fjs/Fiber.h>
#ifdef _WIN32
#include <Windows.h>
#endif

// TODO: Add exceptiosn for invalid stuff?

static void LaunchFiber(fjs::Fiber* fiber)
{
	auto callback = fiber->GetCallback();
	callback(fiber);
}

fjs::Fiber::Fiber()
{
#ifdef _WIN32
	m_fiber = CreateFiber(0, (LPFIBER_START_ROUTINE)LaunchFiber, this);
#endif
}

fjs::Fiber::~Fiber()
{
#ifdef _WIN32
	if (m_fiber)
		DeleteFiber(m_fiber);
#endif
}

void fjs::Fiber::SwitchTo(fjs::Fiber* fiber, void* userdata)
{
	fiber->m_userdata = userdata;
	fiber->m_return_fiber = this;

	SwitchToFiber(fiber->m_fiber);
}

void fjs::Fiber::SwitchBack()
{
	SwitchToFiber(m_return_fiber);
}

fjs::Fiber fjs::Fiber::ConvertCurrentThread()
{
#ifdef _WIN32
	return Fiber(ConvertThreadToFiber(nullptr));
#endif
}