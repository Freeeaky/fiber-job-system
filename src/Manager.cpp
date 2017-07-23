#include <fjs/Manager.h>
#include <fjs/Thread.h>
#include <fjs/Fiber.h>
#include <thread>

fjs::Manager::Manager(uint8_t numThreads, uint16_t numFibers) :
	m_numThreads(numThreads), m_numFibers(numFibers)
{}

fjs::Manager::ReturnCode fjs::Manager::Run(Main_t main)
{
	if (m_threads || m_fibers)
		return ReturnCode::AlreadyInitialized;

	// Threads
	if (m_numThreads == 0)
		m_numThreads = std::thread::hardware_concurrency();

	m_threads = new Thread[m_numThreads];

	// Current (Main) Thread
	m_threads[0] = CurrentThread();

	auto mainThread = &m_threads[0];
	auto mainThreadTLS = mainThread->GetTLS();
	mainThreadTLS->ThreadFiber = Thread::ConvertToFiber();

	// Create Fibers
	// This has to be done after Thread is converted to Fiber!
	if (m_numFibers == 0)
		return ReturnCode::InvalidNumFibers;

	m_fibers = new Fiber[m_numFibers];
	for (uint16_t i = 0; i < m_numFibers; i++)
		m_fibers[i].Reset(nullptr); // TODO callback

	// Spawn Threads
	for (uint8_t i = 1; i < m_numThreads; i++) // offset 1 because 0 is current thread
		m_threads[i].Spawn(nullptr); // TODO callback

	// Main
	if (main == nullptr)
		return ReturnCode::NullCallback;
	
	// Setup main Fiber
	mainThreadTLS->CurrentFiberIndex = FindFreeFiber();
	auto mainFiber = &m_fibers[mainThreadTLS->CurrentFiberIndex];
	mainFiber->Reset(nullptr); // TODO callback
	
	mainThreadTLS->ThreadFiber.SwitchTo(mainFiber);
	
	// Done
	return ReturnCode::Succes;
}