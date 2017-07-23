#include <fjs/Manager.h>
#include <fjs/Counter.h>
#include <iostream>
#include <Windows.h>

void test_job_1(void* ud)
{
	std::cout << "test_job_1 " << (const char*)ud << std::endl;
}

fjs::Job job_low_prio(test_job_1, "low prio");
fjs::Job job_normal_prio(test_job_1, "normal prio");
fjs::Job job_high_prio(test_job_1, "high prio");

void main_test(fjs::Manager* mgr)
{
	fjs::Counter ctr(mgr);
	job_high_prio.counter = &ctr;

	for(int i = 0; i < 15; i++)
		mgr->ScheduleJob(fjs::JobPriority::High, job_high_prio);

	mgr->WaitForCounter(&ctr);
}

int main()
{
	// Setup Job Manager
	fjs::ManagerOptions managerOptions;
	managerOptions.NumFibers = managerOptions.NumThreads * 10;
	
	managerOptions.HighPriorityQueueSize = 128;
	managerOptions.NormalPriorityQueueSize = 256;
	managerOptions.LowPriorityQueueSize = 256;

	// Manager
	fjs::Manager manager(managerOptions);

	if (manager.Run(main_test) != fjs::Manager::ReturnCode::Succes)
		std::cout << "oh no" << std::endl;
	else
		std::cout << "done" << std::endl;

	// Don't close
	getchar();
	return 0;
}