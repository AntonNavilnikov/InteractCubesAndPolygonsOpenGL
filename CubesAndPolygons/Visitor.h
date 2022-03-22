#pragma once

struct Cube;
struct Polygon2D;

class Visitor
{
public:
    Visitor() {}
    virtual ~Visitor() {}

    virtual void VisitCube(const Cube *cube) = 0;
    virtual void VisitPolygon2D(const Polygon2D* polygon) = 0;
};

