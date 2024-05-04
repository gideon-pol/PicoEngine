#pragma once
#include <map>
#include <string>
#include "common.h"

namespace Time {
    namespace {
        uint64_t frameCount = 0;
        uint64_t lastFrameTime = 0;
        uint64_t deltaTime = 0;
    };

    void Init();
    void Tick();
    void Sleep(uint64_t microseconds);
    uint64_t GetFrameCount();
    float GetDeltaTime();
    uint64_t GetDeltaTimeMicroseconds();
    float Now();
    uint64_t NowMicroseconds();

    namespace Profiler {
        namespace {
            // int current = 0;

            std::map<std::string, uint64_t> times_map;
            std::map<std::string, uint64_t> last_enter_map;
        };

        void Enter(std::string name);
        void Exit(std::string name);
        uint64_t GetSectionMicroseconds(std::string name);
        void Print();
        void Reset();
    };

    // float GetDeltaTimeSeconds(){
    //     return (float)deltaTime / 1000000.0f;
    // };

    // float GetDeltaTimeMilliseconds(){
    //     return (float)deltaTime / 1000.0f;
    // };

    // float GetDeltaTimeMicroseconds(){
    //     return (float)deltaTime;
    // };

    // uint64_t GetTime(){
    //     return lastFrameTime;
    // };

    // float GetTimeSeconds(){
    //     return (float)lastFrameTime / 1000000.0f;
    // };

    // float GetTimeMilliseconds(){
    //     return (float)lastFrameTime / 1000.0f;
    // };

    // float GetTimeMicroseconds(){
    //     return (float)lastFrameTime;
    // };
};
