#include <fjs/Manager.h>
#include <fjs/Counter.h>
#include <fjs/List.h>
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
	fjs::List list(mgr, fjs::JobPriority::High);

	for(int i = 0; i < 15; i++)
		list += job_high_prio;

	list.Wait();
}

int main()
{
	// Setup Job Manager
	fjs::ManagerOptions managerOptions;
	managerOptions.NumFibers = managerOptions.NumThreads * 10;
	managerOptions.ThreadAffinity = true;
	
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