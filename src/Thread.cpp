#include <fjs/Thread.h>
#include <fjs/Fiber.h>
#ifdef _WIN32
#include <Windows.h>
#endif

fjs::CurrentThread::CurrentThread()
{
	m_handle = GetCurrentThread();
	m_id = GetCurrentThreadId();
}

fjs::Fiber fjs::Thread::ConvertToFiber()
{
#ifdef _WIN32
	return Fiber(ConvertThreadToFiber(nullptr));
#endif
}