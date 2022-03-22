#pragma once
#include "Entity.h"

#include <glm/gtc/type_ptr.hpp>

struct Cube : public Entity {
    Cube(glm::vec3 iCenter, double iEdgeLength, glm::vec3 iMainAxis, glm::vec3 iAuxilaryAxis);

    GLsizei GetTrianglesCount() const override;
    void Accept(Visitor *visitor) const override {
        visitor->VisitCube(this);
    }
    void Rotate(float xdiff, float ydiff) override;
    const float* GetColor() const override {
        static float color[4] = { 0.0f, 0.5f, 0.5f, 1.0f };
        return &color[0];
    }

    double edgeLength;
    // defines one of the planes
    glm::vec3 mainAxis;
    glm::vec3 auxilaryAxis;
};