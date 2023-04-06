#ifndef VERNA_MESH_HPP
#define VERNA_MESH_HPP

#include "Vertex.hpp"
#include <viverna/core/BoundingBox.hpp>

#include <cstdint>
#include <vector>

namespace verna {

struct Mesh {
    using index_t = uint32_t;
    std::vector<Vertex> vertices;
    std::vector<index_t> indices;
    BoundingBox bounds;
    void RecalculateNormals();
    void RecalculateBounds();
};

enum class PrimitiveMeshType : uint8_t { Cube, Pyramid, Sphere };

Mesh LoadPrimitiveMesh(PrimitiveMeshType type);

}  // namespace verna

#endif