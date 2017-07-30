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

	// Counter
	fjs::Counter counter(mgr);

	// It's also possible to create a JobInfo yourself
	// First argument can be a Counter
	fjs::JobInfo test_job(&counter, test_job_1, &count);
	mgr->ScheduleJob(fjs::JobPriority::Normal, test_job);
	mgr->WaitForCounter(&counter);

	// List / Queues
	fjs::List list(mgr);
	list.Add(fjs::JobPriority::Normal, test_job_1, &count);
	//list += test_job; This would be unsafe, Jobs might execute in parallel

	list.Wait();

	fjs::Queue queue(mgr, fjs::JobPriority::High); // default Priority is high
	queue.Add(test_job_1, &count);
	queue += test_job; // Safe, Jobs are executed consecutively

	queue.Execute();
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