#ifndef VERNA_MESH_HPP
#define VERNA_MESH_HPP

#include <cstdint>
#include <vector>
#include "Vertex.hpp"

namespace verna {

struct Mesh {
    using index_t = uint32_t;
    std::vector<Vertex> vertices;
    std::vector<index_t> indices;
};

// TODO Mesh LoadMeshFromFile(const std::filesystem::path& filename)

enum class PrimitiveMeshType : uint8_t { Cube, Pyramid, Sphere };

Mesh LoadPrimitiveMesh(PrimitiveMeshType type);

}  // namespace verna

#endif