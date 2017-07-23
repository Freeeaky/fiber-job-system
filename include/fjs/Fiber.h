#pragma once

namespace fjs
{
	class Fiber
	{
	public:
		using Callback_t = void(*)(Fiber*);

	private:
		void* m_fiber = nullptr;
		Fiber* m_return_fiber = nullptr;

		Callback_t m_callback = nullptr;
		void* m_userdata = nullptr;

		Fiber(void* fiber) :
			m_fiber(fiber)
		{};

	public:
		Fiber();
		~Fiber();

		void Reset(Callback_t);

		void SwitchTo(Fiber*, void* = nullptr);
		void SwitchBack();
		
		inline Callback_t GetCallback() const { return m_callback; };
		inline void* GetUserdata() const { return m_userdata; };
		inline bool IsValid() const { return m_fiber && m_callback; };

		// Converts current Thread to a Fiber
		static Fiber ConvertCurrentThread();
	};
}