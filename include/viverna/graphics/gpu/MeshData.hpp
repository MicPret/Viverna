#ifndef VERNA_GPU_MESH_DATA_HPP
#define VERNA_GPU_MESH_DATA_HPP

#include <array>
#include <cstdint>
#include <viverna/maths/Mat4f.hpp>

namespace verna::gpu {

struct MaterialData {
    std::array<int32_t, 8> texture_indices;
    std::array<float, 4> parameters;
};

struct MeshData {
    MaterialData material;
    Mat4f model_matrix;
};
}  // namespace verna::gpu

#endif