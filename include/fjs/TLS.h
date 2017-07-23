#pragma once
#include <stdint.h>

namespace fjs
{
	struct TLS
	{
		TLS() = default;
		~TLS() = default;

		// Thread Fiber
		Fiber ThreadFiber;

		// Current Fiber
		uint16_t CurrentFiberIndex = UINT16_MAX;

		// Previous Fiber
		uint16_t PreviousFiberIndex = UINT16_MAX;
	};
}