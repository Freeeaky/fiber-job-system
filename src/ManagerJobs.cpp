#include <fjs/Common.h>
#include <fjs/Manager.h>
#include <fjs/Counter.h>
#include <fjs/TLS.h>

fjs::detail::JobQueue* fjs::Manager::GetQueueByPriority(JobPriority prio)
{
	switch (prio)
	{
	case JobPriority::High:
		return &m_highPriorityQueue;

	case JobPriority::Normal:
		return &m_normalPriorityQueue;

	case JobPriority::Low:
		return &m_lowPriorityQueue;

	default:
		return nullptr;
	}
}

bool fjs::Manager::GetNextJob(JobInfo& job, TLS* tls)
{
	// High Priority Jobs always come first
	if (m_highPriorityQueue.Dequeue(job))
		return true;

	// Ready Fibers
	if (tls == nullptr)
		tls = GetCurrentTLS();

	for (auto it = tls->ReadyFibers.begin(); it != tls->ReadyFibers.end(); ++it)
	{
		uint16_t fiberIndex = it->first;

		// Make sure Fiber is stored
		if (!it->second->load(std::memory_order_relaxed))
			continue;

		// Erase
		delete it->second;
		tls->ReadyFibers.erase(it);

		// Update TLS
		tls->PreviousFiberIndex = tls->CurrentFiberIndex;
		tls->PreviousFiberDestination = FiberDestination::Pool;
		tls->CurrentFiberIndex = fiberIndex;

		// Switch to Fiber
		tls->ThreadFiber.SwitchTo(&m_fibers[fiberIndex], this);
		CleanupPreviousFiber(tls);

		break;
	}

	// Normal & Low Priority Jobs
	return
		m_normalPriorityQueue.Dequeue(job) ||
		m_lowPriorityQueue.Dequeue(job);
}

void fjs::Manager::ScheduleJob(JobPriority prio, const JobInfo& job)
{
	auto queue = GetQueueByPriority(prio);
	if (queue)
	{
		if (job.m_counter)
			job.m_counter->Increment();

		if (!queue->Enqueue(job))
			throw fjs::Exception("Job Queue is full!");
	}
}

void fjs::Manager::WaitForCounter(Counter* counter, uint32_t targetValue)
{
	if (counter == nullptr || counter->GetValue() == targetValue)
		return;

	auto tls = GetCurrentTLS();
	auto fiberStored = new std::atomic_bool(false);

	if (counter->AddWaitingFiber(tls->CurrentFiberIndex, targetValue, fiberStored))
	{
		// Already done
		delete fiberStored;
		return;
	}

	// Update TLS
	tls->PreviousFiberIndex = tls->CurrentFiberIndex;
	tls->PreviousFiberDestination = FiberDestination::Waiting;
	tls->PreviousFiberStored = fiberStored;

	// Switch to idle Fiber
	tls->CurrentFiberIndex = FindFreeFiber();
	tls->ThreadFiber.SwitchTo(&m_fibers[tls->CurrentFiberIndex], this);

	// Cleanup
	CleanupPreviousFiber();
}