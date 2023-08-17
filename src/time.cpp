#include "time.hpp"
#include "common.h"

#define MICROSECONDS_IN_SECOND 1000000

#ifdef PLATFORM_PICO
#include <pico/stdlib.h>
#include "hardware/timer.h"

void Time::Init(){
    lastFrameTime = time_us_64();
};

void Time::Update(){
    uint64_t currentTime = time_us_64();
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    frameCount++;
};

void Time::Sleep(uint64_t microseconds){
    sleep_us(microseconds);
};

uint64_t Time::GetFrameCount(){
    return frameCount;
};

float Time::GetDeltaTime(){
    return (float)deltaTime / MICROSECONDS_IN_SECOND;
};

uint64_t Time::GetDeltaTimeMicroseconds(){
    return deltaTime;
};

float Time::Now(){
    return (float)time_us_64() / MICROSECONDS_IN_SECOND;
};

uint64_t Time::NowMicroseconds(){
    return time_us_64();
};
#endif
