#include <fjs/Manager.h>
#include <fjs/Thread.h>
#include <fjs/Fiber.h>
#include <thread>

fjs::Manager::Manager(uint8_t numThreads, uint16_t numFibers) :
	m_numThreads(numThreads), m_numFibers(numFibers),
	m_highPriorityQueue(512),
	m_normalPriorityQueue(2048),
	m_lowPriorityQueue(4096)
{}

fjs::Manager::~Manager()
{
	// TODO
}

fjs::Manager::ReturnCode fjs::Manager::Run(Main_t main)
{
	if (m_threads || m_fibers)
		return ReturnCode::AlreadyInitialized;

	// Threads
	if (m_numThreads == 0)
		m_numThreads = std::thread::hardware_concurrency();

	m_threads = new Thread[m_numThreads];

	// Current (Main) Thread
	m_threads[0].FromCurrentThread();

	auto mainThread = &m_threads[0];
	auto mainThreadTLS = mainThread->GetTLS();
	mainThreadTLS->ThreadFiber.FromCurrentThread();

	// Create Fibers
	// This has to be done after Thread is converted to Fiber!
	if (m_numFibers == 0)
		return ReturnCode::InvalidNumFibers;

	m_fibers = new Fiber[m_numFibers];
	m_idleFibers = new std::atomic_bool[m_numFibers];

	for (uint16_t i = 0; i < m_numFibers; i++)
	{
		m_fibers[i].Reset(FiberCallback_Worker);
		m_idleFibers[i].store(true, std::memory_order_relaxed);
	}

	// Spawn Threads
	for (uint8_t i = 1; i < m_numThreads; i++) // offset 1 because 0 is current thread
	{
		if (!m_threads[i].Spawn(ThreadCallback_Worker, this))
			return ReturnCode::OSError;
	}

	// Main
	if (main == nullptr)
		return ReturnCode::NullCallback;

	m_mainCallback = main;
	
	// Setup main Fiber
	mainThreadTLS->CurrentFiberIndex = FindFreeFiber();
	auto mainFiber = &m_fibers[mainThreadTLS->CurrentFiberIndex];
	mainFiber->Reset(FiberCallback_Main);

	mainThreadTLS->ThreadFiber.SwitchTo(mainFiber, this);

	for (uint8_t i = 1; i < m_numThreads; i++)
		m_threads[i].Join();
	
	// Done
	return ReturnCode::Succes;
}

uint16_t fjs::Manager::FindFreeFiber()
{
	while (true)
	{
		for (uint16_t i = 0; i < m_numFibers; i++)
		{
			if (!m_idleFibers[i].load(std::memory_order_relaxed) ||
				!m_idleFibers[i].load(std::memory_order_acquire))
				continue;

			bool expected = true;
			if (std::atomic_compare_exchange_weak_explicit(&m_idleFibers[i], &expected, false, std::memory_order_release, std::memory_order_relaxed)) {
				return i;
			}
		}

		// TODO: Add Debug Counter and error message
	}
}