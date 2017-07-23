# fiber-job-system
This library offers a multi-threaded job-system, powered by fibers. There are three job queues with different priorities. Jobs can wait for each other (which allows synchronization between them).

Based on ideas presented by Christian Gyrling in his 2015 GDC presentation [Parallelizing the Naughty Dog Engine Using Fibers](http://www.gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine)
## Basic Usage
```c++
void job_increment_number(void* userdata)
{
	(*(int*)userdata)++;
}

void main_test(fjs::Manager* mgr)
{
	int number = 999;
	mgr->WaitForSingle(fjs::JobPriority::High, fjs::JobInfo(job_increment_number, &number));

	std::cout << number << std::endl; // prints 1000
}

int main()
{
	fjs::Manager manager;
	if (manager.Run(main_test) != fjs::Manager::ReturnCode::Succes)
		return -1;

	return 0;
}
```
- **fjs::Manager** is a class which manages all Fibers, Threads and Jobs
- **manager.Run** starts all Threads & runs *main_test*. As soon as it returns, all Threads are shut down.

### Job Callbacks
Threre are two different types of Job callbacks:
```c++
using Callback_t = void(*)(void*);

struct Job
{
	virtual ~Job() = default;
	virtual void Execute(void*) = 0;
};
```
### JobInfo Struct
When requesting a Job to be executed, you need to create a JobInfo instance with a callback and optionally with userdata & a counter. You can read more about counters below.
```c++
JobInfo(Job* job, void* userdata = nulltpr, Counter* counter = nullptr);
JobInfo(Callback_t callback, void* userdata = nullptr, Counter* counter = nullptr);
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
};
```

### fjs::Counter
Constructed with a *fjs::Manager*, this class provides an atomic counter. It is incremented by each Job that is scheduled with the counter as a third parameter to *fjs::JobInfo*. Once the Job is finished, the counter is decremented.
```c++
void job_increment_number(void* userdata)
{
	(*(int*)userdata)++;
}

void main_test(fjs::Manager* mgr)
{
	int x = 999;

	fjs::Counter counter(mgr);
	mgr->ScheduleJob(fjs::JobPriority::High, fjs::JobInfo(job_increment_number, &x, &counter));

	mgr->WaitForCounter(&counter, 0);
}
```
*fjs::Manager::WaitForCounter* waits until the specified counter has the given value (= in this case 0).

### fjs::List
Helper class for *fjs::Counter*. Scheduling jobs is done by using *operator+=* or the *Add* function. A default priority can be set in the constructor (2nd parameter, default is JobPriority::Normal).
```c++
void main_test(fjs::Manager* mgr)
{
	int x = 999;

	fjs::List list(mgr, fjs::JobPriority::Normal);
	list += fjs::JobInfo(job_increment_number, &x); // Normal priority
	list.Add(fjs::JobPriority::Low, fjs::JobInfo(job_increment_number, &x)); // Low priority
	list.Wait();
}
```

### fjs::Queue
This class allows Jobs to be executed consecutively. It provides both *operator+=* and *Add* (similar to List). The *Step()* method executes and waits for the first Job in the Queue. The *Execute()* method executes until the Queue is empty. Queues are not thread-safe!
```c++
void main_test(fjs::Manager* mgr)
{
	int x = 999;

	fjs::Queue queue(mgr, fjs::JobPriority::Normal);
	queue += fjs::JobInfo(job_increment_number, &x);
	queue += fjs::JobInfo(job_increment_number, &x);
	queue.Add(fjs::JobPriority::Low, fjs::JobInfo(job_increment_number, &x));
	
	queue.Step(); // execute first
	queue.Execute(); // execute remaining
}
```
