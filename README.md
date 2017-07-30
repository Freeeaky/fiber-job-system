# fiber-job-system
This library offers a multi-threaded job-system, powered by fibers. There are three job queues with different priorities. Jobs can wait for each other (which allows synchronization between them).

Based on ideas presented by Christian Gyrling in his 2015 GDC presentation [Parallelizing the Naughty Dog Engine Using Fibers](http://www.gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine)
## Practical Example
```c++
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
	fjs::Manager manager;
	if (manager.Run(main_test) != fjs::Manager::ReturnCode::Succes)
		return -1;

	return 0;
}
```

### Job Callbacks
Job Callbacks have a few limitations: All arguments must be trivial and the total size of a callback is limited to 32 bytes (x86) or to 64 bytes (x64). (Capturing) Lambdas, Member Functions are also supported.

### JobInfo Struct
The JobInfo Struct holds a Job callback and a Counter. There are a number of constructors supporting all Callback types. In most cases, you are not required to create a JobInfo instance yourself, the only exception are the *operator+=* overrides, read more about them below.

### Scheduling Jobs
```c++
void main_test(fjs::Manager* mgr)
{
	int x = 999;
	mgr->ScheduleJob(fjs::JobPriority::Normal, job_increment_number, &x);
	// NOTE: The execution continues here, the Job is executed in another Thread.
}
```

### JobPriority
```c++
enum class JobPriority : uint8_t
{
	High,		// Jobs are executed ASAP
	Normal,
	Low
};
```

## Advanced Usage
### Creating a fjs::Manager
You can configure your *fjs::Manager* object by passing a *fjs::ManagerOptions* instance to the constructor. Although it is disabled by default, I recommend enabling *ThreadAffinity* to lock each Worker Thread to a Queue. For more information, read http://eli.thegreenplace.net/2016/c11-threads-affinity-and-hyperthreading/
```c++
struct ManagerOptions
{
	// Threads & Fibers
	uint8_t NumThreads;						// Amount of Worker Threads, default = amount of Cores
	uint16_t NumFibers = 25;				// Amount of Fibers
	bool ThreadAffinity = false;			// Lock each Thread to a processor core, requires NumThreads == amount of cores

	// Worker Queue Sizes
	size_t HighPriorityQueueSize   = 512;	// High Priority
	size_t NormalPriorityQueueSize = 2048;	// Normal Priority
	size_t LowPriorityQueueSize    = 4096;	// Low Priority
	
	// Other
	bool ShutdownAfterMainCallback = true;	// Shutdown everything after Main Callback returns?
};
```

### fjs::Counter
Constructed with a *fjs::Manager*, this class provides an atomic counter. It is incremented by each Job that is scheduled with the counter as a third parameter to *fjs::JobInfo*. Once the Job is finished, the counter is decremented.
```c++
void job_increment_number(int* number)
{
	(*number)++;
}

void main_test(fjs::Manager* mgr)
{
	int x = 999;

	fjs::Counter counter(mgr);
	mgr->ScheduleJob(fjs::JobPriority::High, job_increment_number, &x, &counter);

	mgr->WaitForCounter(&counter, 0);
}
```
*fjs::Manager::WaitForCounter* waits until the specified counter has the given value (= in this case 0).

### fjs::List
Helper class for *fjs::Counter*. Scheduling jobs is done by using *operator+=* or the *Add* function. A default priority can be set in the constructor (2nd parameter, default is JobPriority::Normal).
```c++
void main_test(fjs::Manager* mgr)
{
	// NOTE: This example is unsafe since the Jobs might run in parallel, each reading & writing to x.
	int x = 999;

	fjs::List list(mgr, fjs::JobPriority::Normal);
	list += fjs::JobInfo(job_increment_number, &x); // Normal priority
	list.Add(job_increment_number, &x); // Normal Priority
	list.Add(fjs::JobPriority::Low, job_increment_number, &x); // Low priority
	list.Wait();
}
```

### fjs::Queue
This class allows Jobs to be executed consecutively. It provides both *operator+=* and *Add* (similar to List). The *Step()* method executes and waits for the first Job in the Queue. The *Execute()* method executes (and waits) until the Queue is empty. Queues are not thread-safe, do not pass them to other Jobs.
```c++
void main_test(fjs::Manager* mgr)
{
	// NOTE: This example is safe since the Jobs write to x consecutively.
	int x = 999;

	fjs::Queue queue(mgr, fjs::JobPriority::Normal);
	queue += fjs::JobInfo(job_increment_number, &x);
	queue += fjs::JobInfo(job_increment_number, &x);
	queue.Add(fjs::JobPriority::Low, job_increment_number, &x);
	
	queue.Step(); // execute first
	queue.Execute(); // execute remaining
}
```
