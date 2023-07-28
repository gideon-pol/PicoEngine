#pragma once

#include <stdint.h>
#include "mathematics.h"
#include "component.h"

#define MAX_COMPONENTS 8

class Entity {
    public:
        Entity();
        ~Entity();

        vec3f Position;
        vec3f Rotation;
        vec3f Scale;

        template<typename T>
        T* GetComponent();

        template<typename T>
        T* AddComponent();

        void RemoveComponent(Component* component);
    private:
        Component* components[MAX_COMPONENTS];

        uint16_t getFreeComponentIndex();
};