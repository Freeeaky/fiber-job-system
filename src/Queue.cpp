#include <fjs/Queue.h>
#include <fjs/Manager.h>

fjs::Queue::Queue(fjs::Manager* mgr, JobPriority defaultPriority) :
	m_manager(mgr),
	m_defaultPriority(defaultPriority),
	m_counter(mgr)
{}

fjs::Queue::~Queue()
{}

void fjs::Queue::Add(JobPriority prio, JobInfo job)
{
	job.SetCounter(&m_counter);
	m_queue.emplace_back(prio, job);
}

fjs::Queue& fjs::Queue::operator+=(const JobInfo& job)
{
	Add(m_defaultPriority, job);
	return *this;
}

bool fjs::Queue::Step()
{
	if (m_queue.empty())
		return false;

	const auto& job = m_queue.front();
	m_manager->ScheduleJob(job.first, job.second);
	m_manager->WaitForCounter(&m_counter);

	m_queue.erase(m_queue.begin());
	return true;
}

void fjs::Queue::Execute()
{
	while (Step())
		;
}