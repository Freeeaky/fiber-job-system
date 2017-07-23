#include <fjs/Manager.h>

fjs::JobQueue* fjs::Manager::GetQueueByPriority(JobPriority prio)
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

bool fjs::Manager::GetNextJob(Job& job)
{
	return
		m_highPriorityQueue.Dequeue(job) ||
		m_normalPriorityQueue.Dequeue(job) ||
		m_lowPriorityQueue.Dequeue(job);
}

void fjs::Manager::ScheduleJob(JobPriority prio, const Job& job)
{
	auto queue = GetQueueByPriority(prio);
	if (queue)
		queue->Enqueue(job);
}