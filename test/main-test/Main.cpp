#include <fjs/Manager.h>
#include <fjs/Counter.h>
#include <fjs/List.h>
#include <fjs/Queue.h>
#include <iostream>
#include <Windows.h>

struct test_job_1 : fjs::Job
{
	virtual void Execute(void* ud) override
	{
		std::cout << "test_job_1 " << (const char*)ud << std::endl;
	}
};

void main_test(fjs::Manager* mgr)
{
	test_job_1 test_job_1_inst;
	mgr->WaitForSingle(fjs::JobPriority::High, fjs::JobInfo(&test_job_1_inst, "x"));
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

	managerOptions.ShutdownAfterMainCallback = true;

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