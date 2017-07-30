#include <fjs/Job.h>
#include <fjs/Counter.h>

void fjs::JobInfo::Execute()
{
	if (!IsNull())
		GetDelegate()->Call();

	if (m_counter)
		m_counter->Decrement();
}