#pragma once
#include "Entity.h"

#include <vector>

#include <glm/gtc/type_ptr.hpp>

struct Polygon2D : public Entity {
    Polygon2D(const std::vector<glm::vec2> iPoints);

    glm::vec2 GetCenter() const; //as rotation point

    GLsizei GetTrianglesCount() const override;
    void Accept(Visitor *visitor) const override {
        visitor->VisitPolygon2D(this);
    }
    void Rotate(float xdiff, float ydiff) override;
    const float* GetColor() const override {
        static float color[4] = { 1.0f, 0.5f, 0.2f, 1.0f };
        return &color[0];
    }

    std::vector<glm::vec2> points;
};
