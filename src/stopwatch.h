#pragma once


#include <windows.h>
#include <profileapi.h>
#include <cstdint>

/*
    A simple stopwatch.
    This stopwatch is independent of wall clock time.  It sets a relative
    start position whenever you call 'reset()'.
    
    The only function it serves is to tell you the number of seconds since
    the reset() method was called.
*/

//
// This stopwatch uses the windows performance counter mechanism
// The operation of this methods is highly dependent on the clock
// speed of the processor, which can change from between cpu cores
// There have been challenges with this method of counting over the years
// but as of Windows 10, it seems to be fairly reliable
class StopWatch
{
    double fStartTime;
    
    // These are convenience functions that wrap up the 
    // various Windows APIs needed for a high precision timer
    static int64_t GetPerfFrequency()
    {
        int64_t anum;

        int success = QueryPerformanceFrequency((LARGE_INTEGER*) &anum);
        if (success == 0) {
            return 0;   //, ffi.C.GetLastError(); 
        }

        return anum;
    }

    static int64_t GetPerfCounter()
    {
        int64_t anum;

        int success = QueryPerformanceCounter((LARGE_INTEGER*)&anum);
        if (success == 0) {
            //return 0; //--, ffi.C.GetLastError();
            return 1;
        }

        return anum;
    }

    // Take the current number of ticks and turn 
    // it into a number of seconds
    static double GetCurrentTickTime()
    {
        double frequency = 1.0 / GetPerfFrequency();
        int64_t currentCount = GetPerfCounter();
        double seconds = (double)currentCount * frequency;

        return seconds;
    }

public:
    //StopWatch(const StopWatch& other) = default;
    StopWatch()
    {
        reset();
    }

    // return the time as a number of seconds
    // since this value is a double, fractions of
    // seconds can be reported.
    double seconds() const
    {
        double currentTime = GetCurrentTickTime();
        return currentTime - fStartTime;
    }

    // Return the time as number of milliseconds instead of seconds
    double millis() const
    {
        return seconds() * 1000;
    }

    // You can reset the clock, which will cause
    // it to use the current time as the starting point.
    // if you specify an amount for 'alreadydone', then
    // you ask how much time has passed, that amount will 
    // be added to the total.
    void reset(const double alreadyDone=0)
    {
        fStartTime = GetCurrentTickTime();
        fStartTime = fStartTime - alreadyDone;
    }
};

