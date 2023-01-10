#include <viverna/core/Debug.hpp>
#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/GraphicsAPIHelper.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/NativeWindow.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Texture.hpp>
#include <viverna/graphics/Vertex.hpp>
#include <viverna/graphics/gpu/CameraData.hpp>
#include <viverna/graphics/gpu/ImplementationInfo.hpp>
#include <viverna/graphics/gpu/MeshData.hpp>
#include "RenderBatch.hpp"
#include "ShaderBucketMapper.hpp"

#include <algorithm>
#include <utility>
#include <vector>

namespace verna {

struct GlDrawCommand {
    // indices counts
    GLsizei* count;
    // indices offsets
    const GLvoid* const* indices;
    // number of meshes
    GLsizei drawcount;
    // vertices offsets
    const GLint* basevertex;
};

namespace {
ShaderBucketMapper shader_to_bucket;
std::vector<RenderBatch> render_batches;

GLuint vao, vbo, ebo, ubo;
bool initialized = false;

void GenBuffers() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // kinda random big value
    constexpr GLsizeiptr gl_buffer_size = 1 << 21;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, gl_buffer_size, nullptr, GL_DYNAMIC_DRAW);
    // glBufferStorage(GL_ARRAY_BUFFER, gl_buffer_size, nullptr,
    // GL_DYNAMIC_STORAGE_BIT);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gl_buffer_size, nullptr,
                 GL_DYNAMIC_DRAW);
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER,
                 sizeof(gpu::CameraData) + sizeof(gpu::MeshDataBuffer), nullptr,
                 GL_DYNAMIC_DRAW);

    glBindBufferRange(GL_UNIFORM_BUFFER, gpu::CameraData::BLOCK_BINDING, ubo, 0,
                      sizeof(gpu::CameraData));
    glBindBufferRange(GL_UNIFORM_BUFFER, gpu::MeshDataBuffer::BLOCK_BINDING,
                      ubo, sizeof(gpu::CameraData),
                      sizeof(gpu::MeshDataBuffer));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, texture_coords)));
    glEnableVertexAttribArray(1);
}

void DeleteBuffers() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &ubo);
}
}  // namespace

void InitializeRenderer() {
    if (initialized)
        return;
    if (!InitGraphicsAPI()) {
        VERNA_LOGE("InitGraphicsAPI() failed!");
        return;
    }
    GenBuffers();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    initialized = true;
}

void TerminateRenderer() {
    if (!initialized)
        return;
    DeleteBuffers();

    initialized = false;

    TermGraphicsAPI();
}

void Render(const Mesh& mesh,
            const Material& material,
            const Mat4f& transform_matrix,
            ShaderId shader_id) {
    VERNA_ASSERT(initialized);
    VERNA_ASSERT(shader_id.IsValid());
    VERNA_ASSERT(!mesh.vertices.empty() && !mesh.indices.empty());

    auto bucket = shader_to_bucket.FindBucket(shader_id);

    BatchId last_batch_id;
    if (shader_to_bucket.NotFound(bucket)) {
        last_batch_id = static_cast<BatchId>(render_batches.size());
        shader_to_bucket.NewBucket(shader_id).push_back(last_batch_id);
        render_batches.push_back(RenderBatch());
    } else {
        last_batch_id = bucket->back();
    }
    RenderBatch& last_batch = render_batches[last_batch_id];
    if (!last_batch.CanContain(mesh)
        || !last_batch.TryAddMeshData(material, transform_matrix)) {
        last_batch_id = static_cast<BatchId>(render_batches.size());
        bucket->push_back(last_batch_id);
        render_batches.push_back(RenderBatch());
        [[maybe_unused]] bool added =
            last_batch.TryAddMeshData(material, transform_matrix);
        VERNA_ASSERT(added);
    }

    RenderBatch& batch = render_batches[last_batch_id];
    batch.vertices_count[batch.num_meshes] =
        static_cast<int32_t>(mesh.vertices.size());
    batch.indices_count[batch.num_meshes] = mesh.indices.size();
    batch.vertices.insert(batch.vertices.end(), mesh.vertices.begin(),
                          mesh.vertices.end());
    batch.indices.insert(batch.indices.end(), mesh.indices.begin(),
                         mesh.indices.end());
    batch.num_meshes++;
}

void Draw() {
    VERNA_ASSERT(initialized);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (shader_to_bucket.Empty())
        return;
    VERNA_ASSERT(!render_batches.empty());

    Camera& cam = Camera::GetActive();
    gpu::CameraData cam_data;
    cam_data.projection_matrix = cam.GetProjectionMatrix();
    cam_data.view_matrix = cam.GetViewMatrix();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(gpu::CameraData), &cam_data);
    ShaderId shader;
    Bucket bucket;
    for (size_t i = 0; i < shader_to_bucket.Size(); i++) {
        shader_to_bucket.Get(i, shader, bucket);
        glUseProgram(shader.id);
#if defined(VERNA_ANDROID)
        int draw_id_loc = glGetUniformLocation(shader.id, "gl_DrawID");
#endif
        for (BatchId batch_id : bucket) {
            RenderBatch& batch = render_batches[batch_id];
            std::array<const GLvoid*, RenderBatch::MAX_MESHES> indices_offsets;
            std::array<GLint, RenderBatch::MAX_MESHES> vertices_offsets;
            indices_offsets[0] = static_cast<const GLvoid*>(0);
            vertices_offsets[0] = 0;
            size_t j;
            for (j = 1; j < RenderBatch::MAX_MESHES; j++) {
                indices_offsets[j] =
                    reinterpret_cast<const GLvoid*>(batch.indices_count[j - 1]);
                vertices_offsets[j] = batch.vertices_count[j - 1];
            }

            glBufferSubData(GL_ARRAY_BUFFER, 0,
                            batch.vertices.size() * sizeof(Vertex),
                            batch.vertices.data());
            glBufferSubData(
                GL_ELEMENT_ARRAY_BUFFER, 0,
                batch.indices.size() * sizeof(decltype(batch.indices[0])),
                batch.indices.data());
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(gpu::CameraData),
                            sizeof(gpu::MeshDataBuffer),
                            batch.mesh_data_buffer.data());
            for (j = 0; j < batch.textures.size(); j++) {
                glActiveTexture(GL_TEXTURE0 + j);
                glBindTexture(GL_TEXTURE_2D, batch.textures[j].id);
            }
            GlDrawCommand draw_command;
            draw_command.basevertex = vertices_offsets.data();
            draw_command.drawcount = batch.num_meshes;
            draw_command.indices = indices_offsets.data();
            draw_command.count = batch.indices_count.data();
#if defined(VERNA_ANDROID)
            for (i = 0; i < draw_command.drawcount; i++) {
                glUniform1i(draw_id_loc, i);
                glDrawElementsBaseVertex(
                    GL_TRIANGLES, draw_command.count[i], GL_UNSIGNED_INT,
                    draw_command.indices[i], draw_command.basevertex[i]);
            }
#elif defined(VERNA_DESKTOP)
            glMultiDrawElementsBaseVertex(GL_TRIANGLES, draw_command.count,
                                          GL_UNSIGNED_INT, draw_command.indices,
                                          draw_command.drawcount,
                                          draw_command.basevertex);
#endif
        }
    }
    SwapNativeWindowBuffers();

    for (auto& batch : render_batches)
        batch.Clear();
    shader_to_bucket.Clear();
}
}  // namespace verna