#include "Timer.h"

namespace MultipleDepthSensorsPlatformServer{
	
	Timer::Timer(clock_t startAt){
		this->_startedAt = startAt;
		this->_pausedAt = 0;
		this->_paused = false;
		this->_started = false;
	}
	
	void Timer::Start()
	{
		if( this->_started ){
			return;
		}

		this->_started = true;
		this->_paused = false;

		this->_internal_counter = clock();
	}

	void Timer::Reset()
	{
		this->_paused = false;
		this->_startedAt = clock();
	}

	clock_t Timer::GetTicks_ms()
	{
		if( !this->_started ){
			return 0;
		}

		if( this->_paused ){
			return this->_pausedAt - this->_startedAt;
		}

		double seconds_passed = clock() - this->_internal_counter;

		return (this->_startedAt+seconds_passed);
	}

}