#include <fjs/Fiber.h>
#ifdef _WIN32
#include <Windows.h>
#endif

fjs::Fiber fjs::Fiber::ConvertCurrentThread()
{
#ifdef _WIN32
	return Fiber(ConvertThreadToFiber(nullptr));
#endif
}