#include <fjs/Manager.h>
#include <iostream>
#include <Windows.h>

void test_job_1(void* ud)
{
	std::cout << "test_job_1" << std::endl;
}

void main_test(fjs::Manager* mgr)
{
	mgr->ScheduleJob(fjs::JobPriority::High, fjs::Job(test_job_1));
	Sleep(1000);
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

	getchar();
	return 0;
}