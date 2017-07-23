#pragma once
#include "Queue.h"

namespace fjs
{
	class Job
	{
	public:
		using Callback_t = void(*)(void*);

		Job() = default;
		Job(Callback_t cb, void* ud = nullptr) :
			callback(cb),
			userdata(ud)
		{};

		Job(const Job&) = default;
		~Job() = default;

		Callback_t callback = nullptr;
		void* userdata = nullptr;
	};

	using JobQueue = Queue<Job>;

	enum class JobPriority : uint8_t
	{
		High,
		Normal,
		Low
	};
}