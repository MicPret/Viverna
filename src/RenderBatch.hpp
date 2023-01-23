#ifndef VERNA_RENDER_BATCH_HPP
#define VERNA_RENDER_BATCH_HPP

#include <vector>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Texture.hpp>
#include <viverna/graphics/Vertex.hpp>
#include <viverna/graphics/gpu/MeshData.hpp>

namespace verna {
struct RenderBatch {
    static constexpr size_t MAX_MESHES = gpu::MeshDataBuffer::BUFFER_SIZE;
    // all of the vertices for this batch
    std::vector<Vertex> vertices;
    // all of the indices for this batch
    std::vector<uint32_t> indices;
    // -> vertices offsets (GLint* basevertex)
    std::array<int32_t, MAX_MESHES> vertices_count;
    // -> indices offsets (const GLvoid* const * indices)
    std::array<int32_t, MAX_MESHES> indices_count;
    // per-mesh data
    std::array<gpu::MeshData, MAX_MESHES> mesh_data_buffer;
    // textures
    std::vector<TextureId> textures;
    // number of meshes (GLsizei drawcount)
    unsigned num_meshes = 0;

    // returns -1 if not found
    int32_t GetTextureIndex(TextureId texture) const;
    bool CanContain(const Mesh& mesh) const;
    bool TryAddMeshData(const Material& material, const Mat4f& transform);
    void Clear();
};

inline bool RenderBatch::CanContain(const Mesh& mesh) const {
    // TODO max vertices, indices...
    return num_meshes < MAX_MESHES;
}

inline int32_t RenderBatch::GetTextureIndex(TextureId texture) const {
    for (size_t i = 0; i < textures.size(); i++)
        if (textures[i] == texture)
            return i;
    return -1;
}

inline bool RenderBatch::TryAddMeshData(const Material& material,
                                        const Mat4f& transform) {
    const size_t max_textures =
        static_cast<size_t>(RendererInfo::MaxTextureUnits());
    for (size_t i = 0; i < material.textures.size(); i++) {
        TextureId mat_texture = material.textures[i];
        if (!mat_texture.IsValid())
            continue;
        auto index = GetTextureIndex(mat_texture);
        if (index == -1) {
            if (textures.size() >= max_textures)
                return false;
            index = textures.size();
            textures.push_back(mat_texture);
        }
        mesh_data_buffer[num_meshes].material_texture_indices[i] = index;
    }
    mesh_data_buffer[num_meshes].material_params = material.parameters;
    mesh_data_buffer[num_meshes].model_matrix = transform;
    return true;
}

inline void RenderBatch::Clear() {
    vertices.clear();
    indices.clear();
#ifndef NDEBUG
    constexpr int INVALID_OFFSET = -1;
    vertices_count.fill(INVALID_OFFSET);
    indices_count.fill(INVALID_OFFSET);
#endif
    textures.clear();
    num_meshes = 0;
}

}  // namespace verna

#endif