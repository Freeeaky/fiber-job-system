#pragma once
#include <stdint.h>

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
		// Threads
		uint8_t m_numThreads;
		Thread* m_threads = nullptr;

		// Fibers
		uint16_t m_numFibers;
		Fiber* m_fibers = nullptr;

		// Thread
		uint8_t GetCurrentThreadIndex() const;
		Thread* GetCurrentThread() const;
		TLS* GetCurrentTLS() const;

		// Fiber
		uint16_t FindFreeFiber();

	public:
		Manager(uint8_t numThreads, uint16_t numFibers);
		~Manager();

		ReturnCode Run(Main_t);

		const uint8_t GetNumThreads() const { return m_numThreads; };
		const uint16_t GetNumFibers() const { return m_numFibers; };
	};
}