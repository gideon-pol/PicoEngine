#pragma once
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "maths.h"


void maths_test(){
    assert(vec3f::up == vec3f(0, 1, 0));
    assert(vec3f::forward == vec3f(0, 0, 1));
    assert(vec3f::right == vec3f(1, 0, 0));
    assert(false);

}