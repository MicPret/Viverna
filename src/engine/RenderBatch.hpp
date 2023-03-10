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
    static constexpr size_t MAX_MESHES = gpu::DrawData::MAX_MESHES;
    // all of the vertices for this batch
    std::vector<Vertex> vertices;
    // all of the indices for this batch
    std::vector<uint32_t> indices;
    // -> vertices offsets (GLint* basevertex)
    std::array<int32_t, MAX_MESHES> vertices_count;
    // -> indices offsets (const GLvoid* const * indices)
    std::array<int32_t, MAX_MESHES> indices_count;
    // per-mesh data
    gpu::DrawData draw_data;
    // textures
    std::vector<TextureId> textures;
    // number of meshes (GLsizei drawcount)
    unsigned num_meshes = 0;

    // returns -1 if not found
    int32_t GetTextureIndex(TextureId texture) const;
    bool CanContain(const Mesh& mesh) const;
    void GenerateOffsets(
        std::array<const void*, MAX_MESHES>& indices_off_out,
        std::array<int32_t, MAX_MESHES>& vertices_off_out) const;
    bool TryAddUniformData(const Material& material, const Mat4f& transform);
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

inline void RenderBatch::GenerateOffsets(
    std::array<const void*, RenderBatch::MAX_MESHES>& indices_off_out,
    std::array<int32_t, RenderBatch::MAX_MESHES>& vertices_off_out) const {
    indices_off_out[0] = static_cast<const void*>(0);
    vertices_off_out[0] = 0;
    for (unsigned i = 1; i < num_meshes; i++) {
        unsigned prev = i - 1;
        size_t n = reinterpret_cast<size_t>(indices_off_out[prev]);
        n += static_cast<size_t>(indices_count[prev])
             * sizeof(decltype(indices[0]));
        indices_off_out[i] = reinterpret_cast<const void*>(n);
        vertices_off_out[i] = vertices_off_out[prev] + vertices_count[prev];
    }
}

inline bool RenderBatch::TryAddUniformData(const Material& material,
                                           const Mat4f& transform) {
    const size_t max_textures =
        static_cast<size_t>(RendererInfo::MaxMaterialTextures());
    gpu::MeshData& mesh_data = draw_data[num_meshes];
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
        mesh_data.material.texture_indices[i] = index;
    }
    mesh_data.material.parameters = material.parameters;
    mesh_data.model_matrix = transform;
    mesh_data.transpose_inverse_model_matrix =
        transform.Inverted().Transposed();
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