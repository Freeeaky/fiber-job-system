#pragma once

namespace fjs
{
	class Thread;

	class Fiber
	{
		friend class Thread;

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
		
		inline bool IsValid() { return m_fiber && m_callback; };
	};
}