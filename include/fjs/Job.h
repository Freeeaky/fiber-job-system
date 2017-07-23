#pragma once
#include "Queue.h"

namespace fjs
{
	class Counter;

	class Job
	{
	public:
		using Callback_t = void(*)(void*);

		Job() = default;
		Job(Callback_t cb, void* ud = nullptr, Counter* ctr = nullptr) :
			callback(cb),
			userdata(ud),
			counter(ctr)
		{};

		Job(const Job&) = default;
		~Job() = default;

		Callback_t callback = nullptr;
		void* userdata = nullptr;
		Counter* counter = nullptr;
	};

	using JobQueue = Queue<Job>;

	enum class JobPriority : uint8_t
	{
		High,
		Normal,
		Low
	};
}