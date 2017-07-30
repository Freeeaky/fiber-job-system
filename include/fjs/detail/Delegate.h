#pragma once
#include <tuple>
#include "detail.h"
#include "function_checks.h"

namespace fjs
{
	namespace detail
	{
		struct base_delegate
		{
			virtual ~base_delegate() = default;
			virtual void Call() = 0;
		};

		// callable: function pointer or lambda class
		template <typename TCallable, typename... Args>
		struct delegate_callable : base_delegate
		{
			TCallable m_callable;
			std::tuple<Args...> m_args;

			delegate_callable(TCallable callable, Args... args) :
				m_callable(callable),
				m_args(args...)
			{};

			virtual ~delegate_callable() = default;

			virtual void Call() override
			{
				apply(m_callable, m_args);
			}
		};

		// member: member function
		template <class TClass, typename Ret, typename... Args>
		struct delegate_member : base_delegate
		{
			using function_t = Ret(TClass::*)(Args...);
			function_t m_function;
			TClass* m_instance;
			std::tuple<Args...> m_args;

			delegate_member(function_t function, TClass* inst, Args... args) :
				m_function(function),
				m_instance(inst),
				m_args(args...)
			{};

			virtual ~delegate_member() = default;

			virtual void Call() override
			{
				apply(m_instance, m_function, m_args);
			}
		};

		// delegate
		template <unsigned BufferSize>
		class Delegate
		{
			char m_buffer[BufferSize] = { 0 };
			inline base_delegate* GetDelegate() { return reinterpret_cast<base_delegate*>(m_buffer); };

			template <typename callback_t, typename... Args>
			void StoreDelegate(Args... args)
			{
				delegate_size_checker<sizeof(callback_t), BufferSize>::check();
				new(m_buffer) callback_t(args...);
			}

			template <class TClass, typename... Args>
			void StoreDelegate(TClass* inst, Args... args)
			{
				using Ret = function_traits<TClass>::return_type;
				StoreDelegate<typename delegate_member<TClass, Ret, Args...>>(&TClass::operator(), inst, args...);
			}

		public:
			Delegate() = default;
			~Delegate()
			{
				Reset(nullptr);
			}

			inline bool IsNull()
			{
				return GetDelegate() == nullptr;
			}

			void Reset(std::nullptr_t)
			{
				if (!IsNull())
				{
					GetDelegate()->~base_delegate();
					*(void**)m_buffer = nullptr;
				}
			}

			template <typename TCallable, typename... Args>
			void Reset(TCallable callable, Args... args)
			{
				Reset(nullptr);
				function_checker<TCallable, Args...>::check();
				
				StoreDelegate<typename delegate_callable<TCallable, Args...>>(callable, args...);
			}

			template <typename Ret, typename... Args>
			void Reset(Ret(*function)(Args...), Args... args)
			{
				Reset(nullptr);
				function_checker<decltype(function), Args...>::check();

				StoreDelegate<typename delegate_callable<decltype(function), Args...>>(function, args...);
			}

			template <class TCallable, typename... Args>
			void Reset(TCallable* callable, Args... args)
			{
				Reset(nullptr);

				function_checker<TCallable, Args...>::check();
				StoreDelegate(callable, args...);
			}

			template <class TClass, typename Ret, typename... Args>
			void Reset(Ret(TClass::* callable)(Args...), TClass* inst, Args... args)
			{
				Reset(nullptr);
				function_checker<decltype(callable), TClass*, Args...>::check();

				StoreDelegate<typename delegate_member<TClass, Ret, Args...>>(callable, inst, args...);
			}
		};
	}
}