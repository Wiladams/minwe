#pragma once


#include <functional>

#include "event.h"
#include "thread.h"
#include "stopwatch.h"

using TimedFunc = std::function< void() >;

namespace timing {
	struct TimedRoutine {
		unsigned long mills;
		TimedFunc aFunc;
		size_t iterations;

		TimedRoutine(unsigned long mills, TimedFunc func, size_t iterations)
			:mills(mills),
			aFunc(func),
			iterations(iterations)
		{}

	};

	/*
	static inline DWORD __stdcall runTimedRoutine(void* param)
	{
		TimedRoutine* r = (TimedRoutine*)param;
		Event e;
		e.wait(r->mills);

		r->aFunc();
		delete r;

		return 0;
	}
	*/
	static inline DWORD __stdcall runPeriodicRoutine(void* param)
	{
		TimedRoutine* r = (TimedRoutine*)param;
		Event e;
		StopWatch sw;

		uint64_t nextTime = sw.millis() + (uint64_t)r->mills;

		size_t iteration = 0;
		while (true) {
			if ((r->iterations > 0) && (iteration >= r->iterations))
				break;

			DWORD waitTime = (DWORD)(nextTime - sw.millis());
			e.wait(nextTime - sw.millis());

			r->aFunc();

			nextTime = nextTime + (uint64_t)r->mills;
			
			iteration += 1;
		}

		delete r;

		return 0;
	}

	static inline void sleep(unsigned long millis)
	{
		Event e;
		e.wait(millis);
	}

	static inline std::shared_ptr<Thread> delay(unsigned long millis, TimedFunc func)
	{
		TimedRoutine* task = new TimedRoutine(millis, func, 1);
		auto t = std::make_shared<Thread>(runPeriodicRoutine, task);
		t->resume();

		return t;
	}

	static inline std::shared_ptr<Thread> periodic(unsigned long millis, TimedFunc func)
	{
		TimedRoutine* task = new TimedRoutine(millis, func,0);
		auto t = std::make_shared<Thread>(runPeriodicRoutine, task);
		t->resume();

		return t;
	}
}