#pragma once
#include "renderer/Mesh.h"

class Primitive {
public:
    static Mesh Cube();
    static Mesh Sphere(int sectorCount = 36, int stackCount = 18, float radius = 0.5f);
    //static Mesh Sphere(int subdivisions = 16);
    //static Mesh Plane();

private:
    static const float s_CubeVertices[];
    //static const int   s_CubeVertexCount;
};