#pragma once
#include <stdint.h>
#include "Job.h"

namespace fjs
{
	class Thread;
	struct TLS;
	class Fiber;

	class Manager
	{
	public:
		enum class ReturnCode : uint8_t
		{
			Succes = 0,

			UnknownError,
			OSError,				// OS-API call failed
			NullCallback,			// callback is nullptr

			AlreadyInitialized,		// Manager has already initialized
			InvalidNumFibers,		// Fiber count is 0 or too high
		};

		using Main_t = void(*)(fjs::Manager*);

	protected:
		std::atomic_bool m_shuttingDown = false;

		// Threads
		uint8_t m_numThreads;
		Thread* m_threads = nullptr;

		// Fibers
		uint16_t m_numFibers;
		Fiber* m_fibers = nullptr;
		std::atomic_bool* m_idleFibers = nullptr;

		uint16_t FindFreeFiber();

		// Thread
		uint8_t GetCurrentThreadIndex() const;
		Thread* GetCurrentThread() const;
		TLS* GetCurrentTLS() const;

		// Work Queue
		JobQueue m_highPriorityQueue;
		JobQueue m_normalPriorityQueue;
		JobQueue m_lowPriorityQueue;

		JobQueue* GetQueueByPriority(JobPriority);
		bool GetNextJob(Job&);

	private:
		Main_t m_mainCallback = nullptr;

		static void ThreadCallback_Worker(fjs::Thread*);
		static void FiberCallback_Worker(fjs::Fiber*);
		static void FiberCallback_Main(fjs::Fiber*);

	public:
		Manager(uint8_t numThreads, uint16_t numFibers);
		~Manager();

		// Initialize & Run Manager
		ReturnCode Run(Main_t);

		// Shutdown all Jobs/Threads/Fibers
		// blocking => wait for threads to exit
		void Shutdown(bool blocking);

		// Jobs
		void ScheduleJob(JobPriority, const Job&);

		// Getter
		inline bool IsShuttingDown() const { return m_shuttingDown.load(std::memory_order_acquire); };
		const uint8_t GetNumThreads() const { return m_numThreads; };
		const uint16_t GetNumFibers() const { return m_numFibers; };
	};
}