#ifndef VERNA_GPU_MESH_DATA_HPP
#define VERNA_GPU_MESH_DATA_HPP

#include <array>
#include <cstdint>
#include <viverna/maths/Mat4f.hpp>

namespace verna::gpu {
struct MeshData {
    std::array<int32_t, 8> material_texture_indices;
    std::array<float, 4> material_params;
    Mat4f model_matrix;
};
struct MeshDataBuffer {
    static constexpr unsigned BUFFER_SIZE = 64u;
    std::array<MeshData, BUFFER_SIZE> mesh_data;
    static constexpr uint32_t BLOCK_BINDING = 1;
    static constexpr const char BLOCK_NAME[] = "MeshDataBuffer";
};
}  // namespace verna::gpu

#endif