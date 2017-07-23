#include <fjs/Thread.h>
#include <fjs/Fiber.h>
#ifdef _WIN32
#include <Windows.h>
#endif

bool fjs::Thread::Spawn(Callback_t callback, void* userdata)
{
	m_handle = nullptr; m_id = UINT32_MAX;
	m_userdata = userdata;

#ifdef _WIN32
	m_handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)m_callback, this, 0, (DWORD*) &m_id);
#endif

	return HasSpawned();
}

void fjs::Thread::FromCurrentThread()
{
	m_handle = GetCurrentThread();
	m_id = GetCurrentThreadId();
}