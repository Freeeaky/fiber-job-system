#include <fjs/Job.h>
#include <fjs/Counter.h>

void fjs::JobInfo::Execute()
{
	if (m_callback)
	{
		if (m_trivial)
			m_callback(m_userdata);
		else
			m_job->Execute(m_userdata);
	}

	if (m_counter)
		m_counter->Decrement();
}