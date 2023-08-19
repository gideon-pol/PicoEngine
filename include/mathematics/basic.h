#pragma once

#define PI 3.14159265358979323846f

template<typename T>
constexpr T min(T a, T b){
    return a < b ? a : b;
}

template<typename T>
constexpr T max(T a, T b){
    return a > b ? a : b;
}