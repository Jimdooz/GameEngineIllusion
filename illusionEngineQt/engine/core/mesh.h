#pragma once

#include "common/commonheaders.h"
#include "utilities/utilities.h"
#include "utilities/mathtypes.h"

using namespace illusion::math;
using namespace illusion::utl;

namespace illusion::core {

    class Mesh
    {
    public:
        Mesh(std::vector<Vector3> *vertices, std::vector<u32> *triangles, std::vector<Vector2> *uv = nullptr, std::vector<Vector3> *normals = nullptr);
    protected:
        std::vector<Vector3> *vertices;
        std::vector<u32> *triangles;
        std::vector<Vector2> *uv;
        std::vector<Vector3> *normals;
    };

}
