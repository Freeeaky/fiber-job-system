#include <fjs/List.h>
#include <fjs/Manager.h>

fjs::List::List(fjs::Manager* mgr, JobPriority defaultPriority) :
	m_manager(mgr),
	m_defaultPriority(defaultPriority),
	m_counter(mgr)
{}

fjs::List::~List()
{}

void fjs::List::Add(JobPriority prio, JobInfo job)
{
	job.SetCounter(&m_counter);

	m_manager->ScheduleJob(prio, job);
}

fjs::List& fjs::List::operator+=(const JobInfo& job)
{
	Add(m_defaultPriority, job);
	return *this;
}

void fjs::List::Wait(uint32_t targetValue)
{
	m_manager->WaitForCounter(&m_counter, targetValue);
}