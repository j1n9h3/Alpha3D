#pragma once
#include "renderer/Mesh.h"

class Primitive {
public:
    static Mesh Cube();
    //static Mesh Sphere(int subdivisions = 16);
    //static Mesh Plane();

private:
    static const float s_CubeVertices[];
    //static const int   s_CubeVertexCount;
};