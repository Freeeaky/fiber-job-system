#include <fjs/Manager.h>
#include <iostream>
#include <Windows.h>

void main_test(fjs::Manager* mgr)
{
	Sleep(1000);
}

int main()
{
	fjs::Manager manager(0, 20);
	if (manager.Run(main_test) != fjs::Manager::ReturnCode::Succes)
		std::cout << "oh no" << std::endl;
	else
		std::cout << "done" << std::endl;

	return 0;
}