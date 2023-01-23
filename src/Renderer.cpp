#include <viverna/graphics/Renderer.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Texture.hpp>
#include <viverna/graphics/Vertex.hpp>
#include <viverna/graphics/gpu/CameraData.hpp>
#include <viverna/graphics/gpu/MeshData.hpp>
#include "RenderBatch.hpp"
#include "ShaderBucketMapper.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#if defined(VERNA_DESKTOP)
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#elif defined(VERNA_ANDROID)
#include <GLES3/gl32.h>
#include <EGL/egl.h>
#else
#error Platform not supported!
#endif

namespace verna {

namespace {
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

void* native_window = nullptr;
ShaderBucketMapper shader_to_bucket;
std::vector<RenderBatch> render_batches;

GLuint vao, vbo, ebo, ubo;

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

void ClearBatches() {
    /*
    for (auto& batch : render_batches)
        batch.Clear();
    */
    shader_to_bucket.Clear();
    render_batches.clear();
}

void SwapBuffers() {
    VERNA_LOGE_IF(native_window == nullptr,
                  "SwapBuffers called with nullptr native_window");
#if defined(VERNA_DESKTOP)
    GLFWwindow* window = static_cast<GLFWwindow*>(native_window);
    VERNA_LOGE_IF(window != glfwGetCurrentContext(),
                  "Renderer error: no access to current context window!");
    glfwSwapBuffers(window);
    glfwPollEvents();
#elif defined(VERNA_ANDROID)
    EGLDisplay display = eglGetCurrentDisplay();
    EGLSurface surface = eglGetCurrentSurface(EGL_DRAW);
    VERNA_LOGE_IF(display == EGL_NO_DISPLAY,
                  "SwapBuffers called with EGL_NO_DISPLAY");
    VERNA_LOGE_IF(surface == EGL_NO_SURFACE,
                  "SwapBuffers called with EGL_NO_SURFACE");
    eglSwapBuffers(display, surface);
    VERNA_LOGE_IF(eglGetError() != EGL_SUCCESS,
                  "Error found calling eglSwapBuffers");
#endif
}

void RendererError(VivernaState& state,
                   [[maybe_unused]] std::string_view message) {
    VERNA_LOGE(message);
    state.SetFlag(VivernaState::ERROR_FLAG, true);
}

BatchId NewBatchInExistingBucket(Bucket& bucket) {
    BatchId output = static_cast<BatchId>(render_batches.size());
    bucket.push_back(output);
    render_batches.push_back(RenderBatch());
    return output;
}

BatchId NewBatchInNewBucket(ShaderId shader) {
    Bucket& bucket = shader_to_bucket.NewBucket(shader);
    return NewBatchInExistingBucket(bucket);
}
}  // namespace

void InitializeRenderer(VivernaState& state) {
    if (state.GetFlag(VivernaState::RENDERER_INITIALIZED_FLAG))
        return;
    if (!state.GetFlag(VivernaState::RENDERER_API_INITIALIZED_FLAG)) {
        RendererError(state,
                      "InitializeRendererAPI() should be called before "
                      "InitializeRenderer()!");
        return;
    }

    GenBuffers();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    state.SetFlag(VivernaState::RENDERER_INITIALIZED_FLAG, true);
    native_window = state.native_window;
    VERNA_LOGI("Renderer initialized!");
}

void TerminateRenderer(VivernaState& state) {
    if (!state.GetFlag(VivernaState::RENDERER_INITIALIZED_FLAG))
        return;

    DeleteBuffers();

    state.SetFlag(VivernaState::RENDERER_INITIALIZED_FLAG, false);
    native_window = nullptr;
    VERNA_LOGI("Renderer terminated!");
}

void Render(const Mesh& mesh,
            const Material& material,
            const Mat4f& transform_matrix,
            ShaderId shader_id) {
    VERNA_LOGE_IF(!shader_id.IsValid(), "Called Render() with invalid shader!");
    VERNA_LOGE_IF(mesh.vertices.empty() || mesh.indices.empty(),
                  "Called Render() on empty Mesh!");

    auto bucket = shader_to_bucket.FindBucket(shader_id);

    BatchId last_batch_id;
    if (shader_to_bucket.NotFound(bucket)) {
        last_batch_id = NewBatchInNewBucket(shader_id);
    } else {
        last_batch_id = bucket->back();
    }
    RenderBatch* last_batch = &render_batches[last_batch_id];
    if (!last_batch->CanContain(mesh)) {
        last_batch_id = NewBatchInExistingBucket(*bucket);
        last_batch = &render_batches[last_batch_id];
    }
    if (!last_batch->TryAddMeshData(material, transform_matrix)) {
        last_batch_id = NewBatchInExistingBucket(*bucket);
        last_batch = &render_batches[last_batch_id];
        [[maybe_unused]] bool added =
            last_batch->TryAddMeshData(material, transform_matrix);
        VERNA_LOGE_IF(!added,
                      "Renderer error: failed to material/transform data");
    }

    RenderBatch& batch = *last_batch;
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
    if (native_window == nullptr) {
        VERNA_LOGE(
            "Called Draw() before Renderer could access current context!");
        ClearBatches();
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (shader_to_bucket.Empty()) {
        SwapBuffers();
        return;
    }
    VERNA_LOGE_IF(render_batches.empty(),
                  "There are render buckets but no render batches!");

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
        int draw_id_loc = glGetUniformLocation(shader.id, "DRAW_ID");
#endif
        std::array<const GLvoid*, RenderBatch::MAX_MESHES> indices_offsets;
        std::array<GLint, RenderBatch::MAX_MESHES> vertices_offsets;
        indices_offsets[0] = static_cast<const GLvoid*>(0);
        vertices_offsets[0] = 0;
        for (BatchId batch_id : bucket) {
            RenderBatch& batch = render_batches[batch_id];
            size_t j;
            for (j = 1; j < batch.num_meshes; j++) {
                size_t n = reinterpret_cast<size_t>(indices_offsets[j - 1]);
                n += static_cast<size_t>(batch.indices_count[j - 1])
                     * sizeof(uint32_t);
                indices_offsets[j] = reinterpret_cast<const GLvoid*>(n);
                vertices_offsets[j] =
                    vertices_offsets[j - 1] + batch.vertices_count[j - 1];
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
            for (j = 0; i < draw_command.drawcount; i++) {
                glUniform1i(draw_id_loc, i);
                glDrawElementsBaseVertex(
                    GL_TRIANGLES, draw_command.count[j], GL_UNSIGNED_INT,
                    draw_command.indices[j], draw_command.basevertex[j]);
            }
#elif defined(VERNA_DESKTOP)
            glMultiDrawElementsBaseVertex(GL_TRIANGLES, draw_command.count,
                                          GL_UNSIGNED_INT, draw_command.indices,
                                          draw_command.drawcount,
                                          draw_command.basevertex);
#endif
        }
    }
    SwapBuffers();

    ClearBatches();
}

namespace RendererInfo {
int MaxTextureUnits() {
    static GLint max_frag_tus = [] {
        GLint x;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &x);
        return x;
    }();
    return max_frag_tus;
}
}  // namespace RendererInfo
}  // namespace verna