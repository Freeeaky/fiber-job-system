#include <fjs/Exception.h>
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

	if (callback == nullptr) {
		throw fjs::Exception("LaunchThread: callback is nullptr");
	}

	thread->WaitForReady();
	callback(thread);
}
#endif

bool fjs::Thread::Spawn(Callback_t callback, void* userdata)
{
	m_handle = nullptr;
	m_id = UINT32_MAX;
	m_callback = callback;
	m_userdata = userdata;
	m_cvReceivedId.notify_all();

#ifdef _WIN32
	{
		std::lock_guard<std::mutex> lock(m_startupIdMutex);
		m_handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LaunchThread, this, 0, (DWORD*)&m_id);
	}
#endif

	return HasSpawned();
}

void fjs::Thread::SetAffinity(size_t i)
{
#ifdef _WIN32
	if (!HasSpawned()) {
		return;
	}

	DWORD_PTR mask = 1ull << i;
	SetThreadAffinityMask(m_handle, mask);
#endif
}

void fjs::Thread::Join()
{
	if (!HasSpawned()) {
		return;
	}

#ifdef _WIN32
	WaitForSingleObject(m_handle, INFINITE);
#endif
}

void fjs::Thread::FromCurrentThread()
{
	m_handle = GetCurrentThread();
	m_id = GetCurrentThreadId();
}

void fjs::Thread::WaitForReady()
{
	// Check if we have an ID already
	{
		std::lock_guard<std::mutex> lock(m_startupIdMutex);
		if (m_id != UINT32_MAX) {
			return;
		}
	}

	// Wait
	std::mutex mutex;

	std::unique_lock<std::mutex> lock(mutex);
	m_cvReceivedId.wait(lock);
}

void fjs::Thread::SleepFor(uint32_t ms)
{
#ifdef _WIN32
	Sleep(ms);
#endif
}