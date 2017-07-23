#include <fjs/Thread.h>
#include <fjs/Fiber.h>
#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef _WIN32
static void WINAPI LaunchThread(void* ptr)
{
	auto thread = reinterpret_cast<fjs::Thread*>(ptr);
	auto callback = thread->GetCallback();
	callback(thread);
}
#endif

bool fjs::Thread::Spawn(Callback_t callback, void* userdata)
{
	m_handle = nullptr; m_id = UINT32_MAX;
	m_callback = callback;
	m_userdata = userdata;

#ifdef _WIN32
	m_handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LaunchThread, this, 0, (DWORD*) &m_id);
#endif

	return HasSpawned();
}

void fjs::Thread::Join()
{
	if (!HasSpawned())
		return;

#ifdef _WIN32
	WaitForSingleObject(m_handle, INFINITE);
#endif
}

void fjs::Thread::FromCurrentThread()
{
	m_handle = GetCurrentThread();
	m_id = GetCurrentThreadId();
}