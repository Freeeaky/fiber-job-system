#pragma once

namespace fjs
{
	class Fiber
	{
		void* m_fiber = nullptr;
		void* m_callback = nullptr;
		void* m_userdata = nullptr;

	private:
		Fiber(void* fiber) :
			m_fiber(fiber)
		{};

	public:
		Fiber() = default;
		~Fiber();

		void SwitchTo(Fiber*, void* = nullptr);
		void Reset(void* callback);
		
		inline bool IsValid() const { return m_fiber && m_callback; };

		// Converts current Thread to a Fiber
		static Fiber ConvertCurrentThread();
	};
}