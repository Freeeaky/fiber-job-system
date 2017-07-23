#pragma once
#include <stdint.h>
#include "TLS.h"

namespace fjs
{
	struct Fiber;

	class Thread
	{
	protected:
		void* m_handle = nullptr;
		uint32_t m_id = UINT32_MAX;
		TLS m_tls;

	public:
		Thread() = default;
		Thread(const Thread&) = delete;
		virtual ~Thread();

		bool Spawn(void* callback);

		inline TLS* GetTLS() { return &m_tls; };
		inline bool HasSpawned() const { return m_id != UINT32_MAX; };
		inline const uint32_t GetID() const { return m_id; };

		// Convert To Fiber
		// Converts current Thread to a Fiber
		static Fiber ConvertToFiber();
	};

	struct CurrentThread : Thread
	{
		CurrentThread();
		virtual ~CurrentThread() = default;
	};
}