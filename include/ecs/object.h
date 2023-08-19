#pragma once

#include "mathematics.h"
#include "time.hpp"

class Object {
    public:
        bool Enabled = true;
        Object* Parent = nullptr;

        Object() : position(vec3f(0, 0, 0)), rotation(Quaternion(0, 0, 0, 1)), scale(vec3f(1, 1, 1)) {};
        Object(const vec3f& position, const Quaternion& rotation, const vec3f& scale) : position(position), rotation(rotation), scale(scale) {};

        FORCE_INLINE constexpr vec3f GetPosition() const {
            return position;
        };

        FORCE_INLINE constexpr Quaternion GetRotation() const {
            return rotation;
        };

        FORCE_INLINE constexpr vec3f GetScale() const {
            return scale;
        };

        FORCE_INLINE constexpr void SetPosition(const vec3f& position) {
            printf("Setting position\n");
            this->position = position;
            translationUpdated = true;
        };

        FORCE_INLINE constexpr void SetRotation(const Quaternion& rotation) {
            printf("Setting rotation\n");
            this->rotation = rotation;
            translationUpdated = true;
        };

        FORCE_INLINE constexpr void SetScale(const vec3f& scale) {
            printf("Setting scale\n");
            this->scale = scale;
            translationUpdated = true;
        };

        mat4f GetModelMatrix() {
            if (translationUpdated) {
                modelMatrix = mat4f::translate(position) * rotation.ToMatrix() * mat4f::scale(scale);
                if(Parent != nullptr) modelMatrix = modelMatrix * Parent->GetModelMatrix();

                translationUpdated = false;
            }

            return modelMatrix;
        };

        FORCE_INLINE constexpr void Translate(const vec3f& translation) {
            position += translation;
            translationUpdated = true;
        };

        FORCE_INLINE constexpr void Rotate(const vec3f& rotation) {
            this->rotation *= Quaternion::Euler(rotation);
            translationUpdated = true;
        };

        FORCE_INLINE constexpr void Rotate(const Quaternion& rotation) {
            this->rotation *= rotation;
            translationUpdated = true;
        };

        FORCE_INLINE void Scale(const vec3f& scale) {
            this->scale += scale;
            translationUpdated = true;
        };

        FORCE_INLINE vec3f GetRight() {
            return GetModelMatrix()(0).xyz();
        };

        FORCE_INLINE vec3f GetUp() {
            return GetModelMatrix()(1).xyz();
        };
        
        FORCE_INLINE vec3f GetForward() {
            return GetModelMatrix()(2).xyz();
        };

private:
    vec3f position;
    Quaternion rotation;
    vec3f scale;
    bool translationUpdated = true;
    mat4f modelMatrix;
};