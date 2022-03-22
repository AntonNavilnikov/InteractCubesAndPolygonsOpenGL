#include "Polygon2D.h"

#include <glm/gtc/matrix_transform.hpp>

Polygon2D::Polygon2D(const std::vector<glm::vec2> iPoints) : points(iPoints)
{
    if (iPoints.size() < 3)
        return;

    glm::vec3 center = glm::vec3(GetCenter(), 0.0);
    translation = glm::translate(glm::mat4(1.0f), center);

    glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f), -center);
    points.resize(iPoints.size());
    for (int i = 0; i < iPoints.size(); ++i) {
        glm::vec4 vertex = glm::vec4(iPoints[i], 0.0, 1.0);
        vertex = toOrigin * vertex;
        points[i] = glm::vec2(vertex.x, vertex.y);
    }
}

glm::vec2 Polygon2D::GetCenter() const
{
    glm::vec2 minPoint = points[0];
    glm::vec2 maxPoint = points[0];
    for (int i = 1; i < points.size(); ++i) {
        if (points[i].x < minPoint.x)
            minPoint.x = points[i].x;
        if (points[i].y < minPoint.y)
            minPoint.y = points[i].y;
        if (points[i].x > maxPoint.x)
            maxPoint.x = points[i].x;
        if (points[i].y > maxPoint.y)
            maxPoint.y = points[i].y;
    }

    return glm::vec2(minPoint.x + (maxPoint.x - minPoint.x) / 2.0, minPoint.y + (maxPoint.y - minPoint.y) / 2.0);
}

GLsizei Polygon2D::GetTrianglesCount() const
{
    return (points.size() - 2);
}

void Polygon2D::Rotate(float xdiff, float ydiff)
{
    glm::mat4 rotation_z = glm::rotate(glm::mat4(1.0f), xdiff + ydiff, glm::vec3(0.0, 0.0, 1.0));
    rotation = rotation_z * rotation;
}
