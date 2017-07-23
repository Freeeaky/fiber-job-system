#pragma once
#include "Counter.h"
#include "Job.h" // Priority
#include <vector>

namespace fjs
{
	class Manager;
	class Counter;

	class Queue
	{
		Manager* m_manager;
		JobPriority m_defaultPriority;

		Counter m_counter;
		std::vector<std::pair<JobPriority, Job>> m_queue;

	public:
		Queue(Manager*, JobPriority defaultPriority = JobPriority::Normal);
		~Queue();

		// Add
		void Add(JobPriority, Job&);
		Queue& operator+=(Job&);

		// Execute all Jobs in Queue
		void Execute();

		// Execute first Job in Queue
		// returns false if queue is empty
		bool Step();
	};
}