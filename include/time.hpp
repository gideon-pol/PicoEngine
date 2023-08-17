#pragma once

#include "common.h"

namespace Time {
    namespace {
        uint64_t frameCount = 0;
        uint64_t lastFrameTime = 0;
        uint64_t deltaTime = 0;
    };

    void Init();
    void Update();
    void Sleep(uint64_t microseconds);
    uint64_t GetFrameCount();
    float GetDeltaTime();
    uint64_t GetDeltaTimeMicroseconds();
    float Now();
    uint64_t NowMicroseconds();

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
