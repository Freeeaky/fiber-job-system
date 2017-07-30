#include <fjs/Manager.h>
#include <fjs/Counter.h>
#include <fjs/List.h>
#include <fjs/Queue.h>
#include <iostream>
#include <Windows.h>

void test_job_1(int* x)
{
	std::cout << "test_job_1 with " << *x << std::endl;
	(*x)++;
}

struct test_job_2
{
	void Execute(int* x)
	{
		std::cout << "test_job_2::Execute with " << *x << std::endl;
		(*x)++;
	}

	void operator()(int* x)
	{
		std::cout << "test_job_2::operator() with " << *x << std::endl;
		(*x)++;
	}
};

void main_test(fjs::Manager* mgr)
{
	int count = 1;

	// 1: Function
	mgr->WaitForSingle(fjs::JobPriority::Normal, test_job_1, &count);

	// 2: Lambda
	mgr->WaitForSingle(fjs::JobPriority::Normal, [&count]() {
		std::cout << "lambda with " << count << std::endl;
		count++;
	});

	// 3: Member Function
	test_job_2 tj2_inst;
	mgr->WaitForSingle(fjs::JobPriority::Normal, &test_job_2::Execute, &tj2_inst, &count);

	// 3: Class operator()
	mgr->WaitForSingle(fjs::JobPriority::Normal, &tj2_inst, &count);
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