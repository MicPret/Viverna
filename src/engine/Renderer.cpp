#include <viverna/graphics/Renderer.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Texture.hpp>
#include <viverna/graphics/Vertex.hpp>
#include <viverna/graphics/gpu/DrawData.hpp>
#include <viverna/graphics/gpu/FrameData.hpp>
#include "RenderBatch.hpp"
#include "ShaderBucketMapper.hpp"
#include "UniformBuffer.hpp"

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
    const GLsizei* count;
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
Bucket debug_bucket;

GLuint vao, vbo, ebo;
GLsizeiptr vbo_size, ebo_size;

ShaderId wireframe_shader;
#ifndef NDEBUG
std::vector<Vertex> dbg_vertices;
std::vector<Mesh::index_t> dbg_indices;
#endif

void LoadWireframeShader() {
    constexpr std::string_view vertex =
        "layout(location = 0) in vec3 in_position;\n"
        "void main() {\n"
        "  SetMeshIdx();"
        "  gl_Position = camera.pv_matrix * vec4(in_position, 1.0);\n"
        "}";
    constexpr std::string_view fragment =
        "out vec4 out_color;\n"
        "void main() {\n"
        "  out_color = vec4(0.6, 1.0, 0.2, 1.0);\n"
        "}";

    wireframe_shader = LoadShaderFromSource(vertex, fragment);
}

void GenBuffers() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    constexpr GLsizeiptr vbo_size_start =
        sizeof(Vertex) * 8 * RenderBatch::MAX_MESHES;
    constexpr GLsizeiptr ebo_size_start =
        sizeof(decltype(RenderBatch::indices[0])) * 6 * 6
        * RenderBatch::MAX_MESHES;

    vbo_size = vbo_size_start;
    ebo_size = ebo_size_start;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_DYNAMIC_DRAW);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_size, nullptr, GL_DYNAMIC_DRAW);
    ubo::GenerateUBO();
    ubo::AddBlock(gpu::FrameData::BLOCK_BINDING, sizeof(gpu::FrameData));
    ubo::AddBlock(gpu::DrawData::BLOCK_BINDING, sizeof(gpu::DrawData));

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
    ubo::TerminateUBO();
    vbo_size = 0;
    ebo_size = 0;
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

void SendDataToVbo(const RenderBatch& batch) {
    const GLsizeiptr vbo_bytes =
        static_cast<GLsizeiptr>(batch.vertices.size() * sizeof(Vertex));
    if (vbo_bytes <= vbo_size) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, vbo_bytes, batch.vertices.data());
    } else {
        vbo_size = std::max(vbo_size * 3 / 2, vbo_bytes);
        glBufferData(GL_ARRAY_BUFFER, vbo_size, batch.vertices.data(),
                     GL_DYNAMIC_DRAW);
    }
}

void SendDataToEbo(const RenderBatch& batch) {
    const GLsizeiptr ebo_bytes = static_cast<GLsizeiptr>(
        batch.indices.size() * sizeof(decltype(RenderBatch::indices[0])));
    if (ebo_bytes <= ebo_size) {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ebo_bytes,
                        batch.indices.data());
    } else {
        ebo_size = std::max(ebo_size * 3 / 2, ebo_bytes);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_size, batch.indices.data(),
                     GL_DYNAMIC_DRAW);
    }
}

void BindTextures(const RenderBatch& batch) {
#if defined(VERNA_DESKTOP)
    const GLuint* textures = &batch.textures.front().id;
    GLsizei count = static_cast<GLsizei>(batch.textures.size());
    glBindTextures(0, count, textures);
#else
    for (size_t i = 0; i < batch.textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, batch.textures[i].id);
    }
#endif
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
    LoadWireframeShader();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Camera::GetActive().Reset();

    state.SetFlag(VivernaState::RENDERER_INITIALIZED_FLAG, true);
    native_window = state.native_window;
    VERNA_LOGI("Renderer initialized!");
}

void TerminateRenderer(VivernaState& state) {
    if (!state.GetFlag(VivernaState::RENDERER_INITIALIZED_FLAG))
        return;

    FreeShader(wireframe_shader);
    DeleteBuffers();

    state.SetFlag(VivernaState::RENDERER_INITIALIZED_FLAG, false);
    native_window = nullptr;
    VERNA_LOGI("Renderer terminated!");
}

void Render(const Mesh& mesh,
            const Material& material,
            const Mat4f& model_matrix,
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
    if (!last_batch->TryAddUniformData(material, model_matrix)) {
        last_batch_id = NewBatchInExistingBucket(*bucket);
        last_batch = &render_batches[last_batch_id];
        [[maybe_unused]] bool added =
            last_batch->TryAddUniformData(material, model_matrix);
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

#ifndef NDEBUG
static void DrawDebug() {
#if defined(VERNA_DESKTOP)
    constexpr GLenum gl_mode = GL_TRIANGLES;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
    constexpr GLenum gl_mode = GL_POINTS;
#endif
    glBufferSubData(GL_ARRAY_BUFFER, 0, dbg_vertices.size() * sizeof(Vertex),
                    dbg_vertices.data());
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    dbg_indices.size() * sizeof(Mesh::index_t),
                    dbg_indices.data());
    glUseProgram(wireframe_shader.id);
    glDrawElements(gl_mode, dbg_indices.size(), GL_UNSIGNED_INT, nullptr);
#if defined(VERNA_DESKTOP)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    dbg_vertices.clear();
    dbg_indices.clear();
}
#endif

static void DrawGlCommand(const GlDrawCommand& cmd) {
#if defined(VERNA_ANDROID)
    constexpr GLint draw_id_uniloc = 0;
    for (size_t j = 0; j < cmd.drawcount; j++) {
        glUniform1i(draw_id_uniloc, j);
        glDrawElementsBaseVertex(GL_TRIANGLES, cmd.count[j], GL_UNSIGNED_INT,
                                 cmd.indices[j], cmd.basevertex[j]);
    }
#elif defined(VERNA_DESKTOP)
    glMultiDrawElementsBaseVertex(GL_TRIANGLES, cmd.count, GL_UNSIGNED_INT,
                                  cmd.indices, cmd.drawcount, cmd.basevertex);
#endif
}

static void DrawBatch(const RenderBatch& batch) {
    static std::array<const GLvoid*, RenderBatch::MAX_MESHES> indices_offsets;
    static std::array<GLint, RenderBatch::MAX_MESHES> vertices_offsets;

    SendDataToVbo(batch);
    SendDataToEbo(batch);
    ubo::SendData(gpu::DrawData::BLOCK_BINDING, &batch.draw_data);
    BindTextures(batch);

    GlDrawCommand draw_command;
    batch.GenerateOffsets(indices_offsets, vertices_offsets);
    draw_command.basevertex = vertices_offsets.data();
    draw_command.drawcount = batch.num_meshes;
    draw_command.indices = indices_offsets.data();
    draw_command.count = batch.indices_count.data();
    DrawGlCommand(draw_command);
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
    gpu::FrameData frame_data;
    gpu::CameraData& cam_data = frame_data.camera_data;
    cam_data.projection_matrix = cam.GetProjectionMatrix();
    cam_data.view_matrix = cam.GetViewMatrix();
    cam_data.pv_matrix = cam_data.projection_matrix * cam_data.view_matrix;
    ubo::SendData(gpu::FrameData::BLOCK_BINDING, &frame_data);

    ShaderId shader;
    Bucket bucket;
    for (size_t i = 0; i < shader_to_bucket.Size(); i++) {
        shader_to_bucket.Get(i, shader, bucket);
        glUseProgram(shader.id);
        for (BatchId batch_id : bucket) {
            const RenderBatch& batch = render_batches[batch_id];
            DrawBatch(batch);
        }
    }
#ifndef NDEBUG
    DrawDebug();
#endif
    SwapBuffers();

    ClearBatches();
}

void RenderDebug(const BoundingBox& box) {
#ifndef NDEBUG
    Mesh::index_t offset = dbg_vertices.size();
    Transform t;
    t.position = box.Center();
    t.scale = box.Size();
    Mesh cube = LoadPrimitiveMesh(PrimitiveMeshType::Cube);
    for (Vertex& v : cube.vertices) {
        v.position = t.Apply(v.position);
        dbg_vertices.push_back(v);
    }
    for (Mesh::index_t id : cube.indices) {
        dbg_indices.push_back(id + offset);
    }
#endif
}

void RenderDebug(const BoundingSphere& sphere) {
#ifndef NDEBUG
    Mesh::index_t offset = dbg_vertices.size();
    Transform t;
    t.position = sphere.Position();
    t.scale = Vec3f(sphere.Radius());
    Mesh mesh = LoadPrimitiveMesh(PrimitiveMeshType::Sphere);
    for (Vertex& v : mesh.vertices) {
        v.position = t.Apply(v.position);
        dbg_vertices.push_back(v);
    }
    for (Mesh::index_t id : mesh.indices) {
        dbg_indices.push_back(id + offset);
    }
#endif
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