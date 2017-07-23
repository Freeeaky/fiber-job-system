#include <fjs/Manager.h>
#include <fjs/Thread.h>
#include <fjs/Fiber.h>

void fjs::Manager::ThreadCallback_Worker(fjs::Thread* thread)
{
	auto manager = reinterpret_cast<fjs::Manager*>(thread->GetUserdata());
	auto tls = thread->GetTLS();
	
	//auto fiber = &manager->m_fibers[tls->CurrentFiberIndex];
	//tls->ThreadFiber.SwitchTo(fiber);
}

void fjs::Manager::FiberCallback_Main(fjs::Fiber* fiber)
{
	auto manager = reinterpret_cast<fjs::Manager*>(fiber->GetUserdata());
	
	// Main
	manager->m_mainCallback(manager);

	// TODO: Notify of quit

	// Switch back to Thread
	fiber->SwitchBack();
}

void fjs::Manager::FiberCallback_Worker(fjs::Fiber* fiber)
{
	auto manager = reinterpret_cast<fjs::Manager*>(fiber->GetUserdata());

	while (true)
	{
		auto tls = manager->GetCurrentTLS();
	}
	
	// Switch back to Thread
	fiber->SwitchBack();
}