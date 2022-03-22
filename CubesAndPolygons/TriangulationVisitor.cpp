#include "TriangulationVisitor.h"
#include "Cube.h"
#include "Polygon2D.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static int isVertexInsideNewPoly(int n, const std::vector<glm::vec2> &p)
{
    glm::vec2 v = p[n];
    std::vector<glm::vec2> a;

    for (size_t i = 0; i < p.size(); i++)
        if (i != n)
            a.push_back(p[i]);

    int c = 1;

    for (size_t i = 0, j = a.size() - 1; i < a.size(); j = i++) 
    {
        if ((((a[i].y <= v.y) && (v.y < a[j].y)) || ((a[j].y <= v.y) && (v.y < a[i].y))) && (v.x > (a[j].x - a[i].x) * (v.y - a[i].y) / (a[j].y - a[i].y) + a[i].x))
            c = !c;
    }

    return c;
}

static int isEdgeIntersect(int n, const std::vector<glm::vec2> &p)
{
    glm::vec2 v = p[n];
    std::vector<glm::vec2> a;

    for (size_t i = 0; i < p.size(); i++)
        if (i != n)
            a.push_back(p[i]);

    int c = 0, cnt = a.size(), prev = (cnt + (n - 1)) % cnt, next = n % cnt;

    glm::vec2 v1 = a[prev], v2 = a[next];

    for (size_t i = 0, j = cnt - 1; i < cnt; j = i++)
    {
        if (prev == i || prev == j || next == i || next == j)
            continue;

        glm::vec2 v4 = a[j], v3 = a[i];

        float denominator = ((v4.y - v3.y) * (v2.x - v1.x)) - ((v4.x - v3.x) * (v2.y - v1.y));

        if (!denominator)
            continue;

        float ua = (((v4.x - v3.x) * (v1.y - v3.y)) - ((v4.y - v3.y) * (v1.x - v3.x))) / denominator;
        float ub = (((v2.x - v1.x) * (v1.y - v3.y)) - ((v2.y - v1.y) * (v1.x - v3.x))) / denominator;

        if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1)
        {
            c = 1;
            break;
        }
    }

    return c;
}

static int isVertexEar(int n, const std::vector<glm::vec2> &p)
{
    return (isVertexInsideNewPoly(n, p) && !isEdgeIntersect(n, p));
}

void TriangulationVisitor::VisitCube(const Cube* cube)
{
    if (std::abs(cube->edgeLength < precision) ||
        cube->auxilaryAxis.length() < precision ||
        cube->auxilaryAxis.length() < precision)
        return;

    glm::vec3 axis1 = cube->mainAxis;
    glm::vec3 axis2 = glm::cross(axis1, cube->auxilaryAxis);
    glm::vec3 axis3 = glm::cross(axis1, axis2);

    std::vector<glm::vec3> points(8);

    double half_edge = cube->edgeLength / 2.0;
    axis1 = glm::normalize(axis1);
    axis1 *= half_edge;
    axis2 = glm::normalize(axis2);
    axis2 *= half_edge;
    axis3 = glm::normalize(axis3);
    axis3 *= half_edge;

    glm::vec3 origin(0.0, 0.0, 0.0);
    points[0] = origin + axis1 + axis2 + axis3;
    points[1] = origin + axis1 + axis2 - axis3;
    points[2] = origin + axis1 - axis2 + axis3;
    points[3] = origin + axis1 - axis2 - axis3;
    points[4] = origin - axis1 + axis2 + axis3;
    points[5] = origin - axis1 + axis2 - axis3;
    points[6] = origin - axis1 - axis2 + axis3;
    points[7] = origin - axis1 - axis2 - axis3;

    AddRectangleToBuffer(points[0], points[1], points[2], points[3]);
    AddRectangleToBuffer(points[1], points[5], points[3], points[7]);
    AddRectangleToBuffer(points[5], points[4], points[7], points[6]);
    AddRectangleToBuffer(points[4], points[0], points[6], points[2]);
    AddRectangleToBuffer(points[0], points[4], points[1], points[5]);
    AddRectangleToBuffer(points[6], points[2], points[7], points[3]);
}

void TriangulationVisitor::VisitPolygon2D(const Polygon2D* polygon)
{
    std::vector<glm::vec2> vertices = polygon->points;

    for (size_t t = vertices.size() - 1, i = 0, j = 1; i < vertices.size(); t = i++, j = (i + 1) % vertices.size())
    {
        if (vertices.size() == 3)
        {
            AddVertexToBuffer(glm::vec3(vertices[0], 0.0));
            AddVertexToBuffer(glm::vec3(vertices[1], 0.0));
            AddVertexToBuffer(glm::vec3(vertices[2], 0.0));
            break;
        }

        if (isVertexEar(i, vertices))
        {
            AddVertexToBuffer(glm::vec3(vertices[t], 0.0));
            AddVertexToBuffer(glm::vec3(vertices[i], 0.0));
            AddVertexToBuffer(glm::vec3(vertices[j], 0.0));

            vertices.erase(vertices.begin() + i, vertices.begin() + i + 1);

            t = vertices.size() - 1;
            i = 0;
            j = 1;
        }
    }
}

void TriangulationVisitor::AddVertexToBuffer(const glm::vec3& point)
{
    buffer[index++] = point.x;
    buffer[index++] = point.y;
    buffer[index++] = point.z;
}

void TriangulationVisitor::AddTriangleToBuffer(
    const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
    AddVertexToBuffer(p1);
    AddVertexToBuffer(p2);
    AddVertexToBuffer(p3);
}

void TriangulationVisitor::AddRectangleToBuffer(
    const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4)
{
    AddVertexToBuffer(p1);
    AddVertexToBuffer(p2);
    AddVertexToBuffer(p3);
    AddVertexToBuffer(p3);
    AddVertexToBuffer(p2);
    AddVertexToBuffer(p4);
}