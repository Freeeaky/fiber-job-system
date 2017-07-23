#include <fjs/Manager.h>
#include <Windows.h>

void main_test(fjs::Manager* mgr)
{
	while (true)
		Sleep(1);
}

int main()
{
	MessageBox(0, 0, 0, 0);
	fjs::Manager manager(0, 20);
	auto err = manager.Run(main_test);

	return 0;
}