#include <fjs/Manager.h>
#include <fjs/Thread.h>
#include <fjs/Fiber.h>
#include <fjs/Counter.h>

void fjs::Manager::ThreadCallback_Worker(fjs::Thread* thread)
{
	auto manager = reinterpret_cast<fjs::Manager*>(thread->GetUserdata());
	auto tls = thread->GetTLS();

	// Thread Affinity
	if (tls->SetAffinity)
		thread->SetAffinity(tls->ThreadIndex);

	// Setup Thread Fiber
	tls->ThreadFiber.FromCurrentThread();

	// Fiber
	tls->CurrentFiberIndex = manager->FindFreeFiber();

	auto fiber = &manager->m_fibers[tls->CurrentFiberIndex];
	tls->ThreadFiber.SwitchTo(fiber, manager);
}

void fjs::Manager::FiberCallback_Main(fjs::Fiber* fiber)
{
	auto manager = reinterpret_cast<fjs::Manager*>(fiber->GetUserdata());
	
	// Main
	manager->m_mainCallback(manager);

	// Shutdown after Main
	if (!manager->m_shutdownAfterMain)
	{
		// Switch to idle Fiber
		auto tls = manager->GetCurrentTLS();
		tls->CurrentFiberIndex = manager->FindFreeFiber();
		
		auto fiber = &manager->m_fibers[tls->CurrentFiberIndex];
		tls->ThreadFiber.SwitchTo(fiber, manager);
	}

	// Shutdown
	manager->Shutdown(false);

	// Switch back to Main Thread
	fiber->SwitchBack();
}

void fjs::Manager::FiberCallback_Worker(fjs::Fiber* fiber)
{
	auto manager = reinterpret_cast<fjs::Manager*>(fiber->GetUserdata());
	manager->CleanupPreviousFiber();

	JobInfo job;

	while (!manager->IsShuttingDown())
	{
		auto tls = manager->GetCurrentTLS();
		
		if (manager->GetNextJob(job, tls))
		{
			job.Execute();
			continue;
		}

		Thread::Sleep(1);
	}
	
	// Switch back to Thread
	fiber->SwitchBack();
}