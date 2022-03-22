#pragma once
#include "Visitor.h"

#include <vector>

#include <GL/glew.h>
#include <glm/detail/type_vec.hpp>

class TriangulationVisitor : public Visitor
{
public:
    TriangulationVisitor(std::vector<GLfloat>& iBuffer, size_t iIndex) : buffer(iBuffer), index(iIndex) {}

    void VisitCube(const Cube *cube) override;
    void VisitPolygon2D(const Polygon2D* polygon) override;

private:
    void AddVertexToBuffer(const glm::vec3& point);
    void AddTriangleToBuffer(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
    void AddRectangleToBuffer(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4);

    double precision = 1e-10;
    std::vector<GLfloat>& buffer;
    size_t index;
};

