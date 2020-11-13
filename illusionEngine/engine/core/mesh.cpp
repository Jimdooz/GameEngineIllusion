#include "mesh.h"

namespace illusion::core {

    Mesh::Mesh(std::vector<Vector3> *vertices, std::vector<u32> *triangles, std::vector<Vector2> *uv, std::vector<Vector3> *normals)
        : vertices(vertices), triangles(triangles), uv(uv), normals(normals)
    {

    }

}
