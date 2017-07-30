#include <fjs/Manager.h>
#include <fjs/Counter.h>
#include <fjs/List.h>
#include <fjs/Queue.h>
#include <iostream>
#include <Windows.h>

#include <fjs/detail/Delegate.h>

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

void test(int* x)
{
	std::cout << "test " << x << std::endl;
}

struct some_struct
{
	int a = 123;
	int b = 234;
	int c = 345;
};

struct non_trivial_struct
{
	non_trivial_struct() {};
};

class some_class
{
public:
	void execute()
	{
		std::cout << "executing" << std::endl;
	}

	void operator()()
	{

	}
};

int main()
{
	using Delegate = fjs::detail::Delegate<64>;
	static constexpr size_t x = sizeof(Delegate);
	int result = 0;

	Delegate dtest;
	//dtest.Reset(dtest);
	dtest.Reset(test, &result);
	
	int a = 0, b = 1, c = 2, d = 3, e = 4, f = 5;

	auto lambda = [&a, b, c, d](int* r, int y) {
		a = b;
		*r = b + c + d * y; // 1 + 2 + 3
	};

	dtest.Reset(lambda, &result, 14);

	some_class inst;
	dtest.Reset(&some_class::execute, &inst);
	dtest.Reset(&inst);

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