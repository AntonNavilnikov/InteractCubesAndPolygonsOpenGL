#include "Cube.h"

#include <glm/gtc/matrix_transform.hpp>

Cube::Cube(glm::vec3 iCenter, double iEdgeLength, glm::vec3 iMainAxis, glm::vec3 iAuxilaryAxis)
: edgeLength(iEdgeLength), mainAxis(iMainAxis), auxilaryAxis(iAuxilaryAxis)
{
    translation = glm::translate(glm::mat4(1.0f), iCenter);
}

GLsizei Cube::GetTrianglesCount() const
{
    return 12;
}

void Cube::Rotate(float xdiff, float ydiff)
{
    glm::mat4 rotation_x = glm::rotate(glm::mat4(1.0f), xdiff, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 rotation_y = glm::rotate(glm::mat4(1.0f), ydiff, glm::vec3(1.0, 0.0, 0.0));
    rotation = rotation_y * (rotation_x * rotation);
}
