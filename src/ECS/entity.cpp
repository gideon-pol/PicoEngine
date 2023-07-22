#include "entity.h"

uint16_t Entity::getFreeComponentIndex(){
    for(int i = 0; i < MAX_COMPONENTS; i++){
        if(components[i] == NULL){
            return i;
        }
    }

    return -1;
}

template<typename T>
T* Entity::GetComponent(){
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

    for(int i = 0; i < MAX_COMPONENTS; i++){
        if(components[i]->get_type() == T::type){
            return (T*)components[i];
        }
    }
    return NULL;
}

template<typename T>
T* Entity::AddComponent(){
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

    uint16_t index = getFreeComponentIndex();
    if(index == -1){
        return NULL;
    }

    T* component = new T();
    components[index] = component;

    return component;
}

void Entity::RemoveComponent(Component* component){
    for(int i = 0; i < MAX_COMPONENTS; i++){
        if(components[i] == component){
            delete components[i];
            components[i] = NULL;
            return;
        }
    }
}