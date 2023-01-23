#ifndef VERNA_MESH_HPP
#define VERNA_MESH_HPP

#include <cstdint>
#include <vector>
#include "Vertex.hpp"

namespace verna {

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

// TODO Mesh LoadMeshFromFile(const std::filesystem::path& filename)

enum class PrimitiveMeshType : uint8_t { Cube, Pyramid };

Mesh LoadPrimitiveMesh(PrimitiveMeshType type);

}  // namespace verna

#endif