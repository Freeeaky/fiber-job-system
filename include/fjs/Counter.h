#pragma once
#include <stdint.h>
#include <atomic>

namespace fjs
{
	class Manager;

	class Counter
	{
		friend class Manager;

	public:
		static const constexpr uint8_t MAX_WAITING = 4;
		using Unit_t = uint32_t;

	private:
		Manager* m_manager;

		// Waiting Fibers
		struct WaitingFibers
		{
			uint16_t FiberIndex = UINT16_MAX;
			std::atomic_bool* FiberStored = nullptr;
			Unit_t TargetValue = 0;

			std::atomic_bool InUse = true;
		} m_waiting[MAX_WAITING];

		std::atomic_bool m_freeWaitingSlots[MAX_WAITING];

		// Counter
		std::atomic<Unit_t> m_counter = 0;

	public:
		Counter(Manager*);
		~Counter() = default;

		// Modifiers
		Unit_t Increment(Unit_t by = 1);
		Unit_t Decrement(Unit_t by = 1);

		Unit_t GetValue() const;
		
	protected:
		bool AddWaitingFiber(uint16_t fiberIndex, Unit_t targetValue, std::atomic_bool* fiberStored);
		void CheckWaitingFibers(Unit_t);
	};
}