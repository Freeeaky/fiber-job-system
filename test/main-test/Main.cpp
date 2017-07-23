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
	fjs::Manager manager(0, 20);
	if (manager.Run(main_test) != fjs::Manager::ReturnCode::Succes)
		std::cout << "oh no" << std::endl;
	else
		std::cout << "done" << std::endl;

	getchar();
	return 0;
}