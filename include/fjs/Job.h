#pragma once
#include "detail_MPMCQueue.h"

namespace fjs
{
	class Counter;

	struct Job
	{
		virtual ~Job() = default;
		virtual void Execute(void*) = 0;
	};

	class JobInfo
	{
	public:
		using Callback_t = void(*)(void*);
		
	private:
		bool m_trivial = true;
		union
		{
			Callback_t m_callback;
			Job* m_job;
		};

	public:
		void* m_userdata = nullptr;
		Counter* m_counter = nullptr;

	public:
		JobInfo() = default;
		JobInfo(Job* j, void* ud = nullptr, Counter* ctr = nullptr) :
			m_trivial(false),
			m_job(j),
			m_userdata(ud),
			m_counter(ctr)
		{};

		JobInfo(Callback_t cb, void* ud = nullptr, Counter* ctr = nullptr) :
			m_trivial(true),
			m_callback(cb),
			m_userdata(ud),
			m_counter(ctr)
		{};

		~JobInfo() = default;

		// Execute Job
		void Execute();
	};

	enum class JobPriority : uint8_t
	{
		High,		// Jobs are executed ASAP
		Normal,
		Low
	};

	namespace detail
	{
		// avoid confusion between fjs::Queue and fjs::JobQueue
		using JobQueue = detail::MPMCQueue<JobInfo>;
	}
}