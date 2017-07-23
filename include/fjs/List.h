#pragma once
#include "Counter.h"
#include "Job.h" // Priority

namespace fjs
{
	class Manager;
	class Counter;

	class List
	{
		Manager* m_manager;
		JobPriority m_defaultPriority;

		Counter m_counter;

	public:
		List(Manager*, JobPriority defaultPriority = JobPriority::Normal);
		~List();

		// Add
		void Add(JobPriority, JobInfo&);
		List& operator+=(JobInfo&);

		// Wait
		void Wait(uint32_t targetValue = 0);
	};
}