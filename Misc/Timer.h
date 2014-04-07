#ifndef TIMER_H
#define TIMER_H

#include <ctime>

using namespace std;

namespace MultipleDepthSensorsServer{

	class Timer
	{
		private:
			clock_t _startedAt;
			clock_t _pausedAt;
			clock_t _internal_counter;
			bool _started;
			bool _paused;
		public:
			Timer::Timer(clock_t startAt);
	
			/* Operations */
			void Start();
			void Reset();
			clock_t GetTicks_ms();
	};
}

#endif