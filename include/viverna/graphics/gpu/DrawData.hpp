#ifndef VERNA_GPU_DRAW_DATA_HPP
#define VERNA_GPU_DRAW_DATA_HPP

#include "MeshData.hpp"

namespace verna::gpu {
struct DrawData {
    static constexpr unsigned MAX_MESHES = 64;

    std::array<MeshData, MAX_MESHES> mesh_data;

    static constexpr uint32_t BLOCK_BINDING = 1;
    static constexpr const char BLOCK_NAME[] = "DrawData";

    constexpr MeshData& operator[](size_t index) { return mesh_data[index]; }
    constexpr const MeshData& operator[](size_t index) const {
        return mesh_data[index];
    }
};
}  // namespace verna::gpu

#endif