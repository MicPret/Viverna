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

// TODO Mesh LoadMeshFromFile(std::string_view filename)

enum class PrimitiveMeshType { Cube };

Mesh LoadPrimitiveMesh(PrimitiveMeshType type);

}  // namespace verna

#endif