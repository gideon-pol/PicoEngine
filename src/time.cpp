#include "time.hpp"
#include "common.h"

#define MICROSECONDS_IN_SECOND 1000000

#ifdef PLATFORM_PICO
#include <pico/stdlib.h>
#include "hardware/timer.h"

void Time::Init(){
    lastFrameTime = time_us_64();
};

void Time::Tick(){
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

// void Time::Profiler::Enter(bool increaseLevel){
//     if(current >= 10) return;

//     last_enter[current] = Now();
//     current++;
// };

// void Time::Profiler::Exit(){
//     if(current <= 0) return;

//     current--;
//     times[current] += Now() - last_enter[current];

//     if(current == 0){
//         for(int i = 0; i < 10; i++){
//             printf("%f\n", times[i]);
//             times[i] = 0;
//         }
//     }
// };

void Time::Profiler::Enter(std::string name){
    last_enter_map[name] = Now();
};

void Time::Profiler::Exit(std::string name){
    times_map[name] += Now() - last_enter_map[name];
};

void Time::Profiler::Print(){
    for(auto& [key, value] : times_map){
        printf("%s: %f\n", key.c_str(), value);
    }
};

void Time::Profiler::Reset(){
    times_map.clear();
};

#ifdef PLATFORM_NATIVE
#include <chrono>
#include <thread>

void Time::Init(){
    lastFrameTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
};

void Time::Tick(){
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    frameCount++;
};

void Time::Sleep(uint64_t microseconds){
    std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
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
    return (float)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() / MICROSECONDS_IN_SECOND;
};

uint64_t Time::NowMicroseconds(){
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
};

#endif
