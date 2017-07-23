#pragma once

namespace fjs
{
	class Job
	{
		void* callback;
		void* userdata;

		char bulk[256];
	};
}