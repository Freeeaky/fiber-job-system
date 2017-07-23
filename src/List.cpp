#include <fjs/List.h>
#include <fjs/Manager.h>

fjs::List::List(fjs::Manager* mgr, JobPriority defaultPriority) :
	m_manager(mgr),
	m_defaultPriority(defaultPriority),
	m_counter(mgr)
{}

fjs::List::~List()
{}

void fjs::List::Add(Job& job)
{
	job.counter = &m_counter;

	m_manager->ScheduleJob(m_defaultPriority, job);
}

fjs::List& fjs::List::operator+=(Job& job)
{
	Add(job);
	return *this;
}

void fjs::List::Wait(uint32_t targetValue)
{
	m_manager->WaitForCounter(&m_counter, targetValue);
}