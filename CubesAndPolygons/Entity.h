#pragma once
#include "Visitor.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

struct Entity
{
    Entity() {}
    virtual ~Entity() {}

    virtual GLsizei GetTrianglesCount() const = 0;
    virtual void Accept(Visitor *visitor) const = 0;
    virtual void Rotate(float xdiff, float ydiff) = 0;
    virtual const float* GetColor() const = 0;

    //if space memory is limited, rotation matrix could be removed and created dynamically,
    //but need to triangulate on the end of rotation
    glm::mat4 rotation;
    glm::mat4 translation;
};

