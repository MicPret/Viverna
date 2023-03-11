#include <viverna/graphics/Renderer.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/core/Transform.hpp>
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

gpu::FrameData frame_data;

ShaderId wireframe_shader;
ShaderId depth_shader;
std::vector<GLuint> depth_maps;
std::vector<GLuint> depth_fbos;
constexpr GLsizei SHADOW_MAP_WIDTH = 1024;
constexpr GLsizei SHADOW_MAP_HEIGHT = 1024;
#ifndef NDEBUG
std::vector<Vertex> dbg_vertices;
std::vector<Mesh::index_t> dbg_indices;
#endif
}  // namespace

static void CheckForGLErrors(std::string_view origin);
static void LoadPrivateShaders();
static void FreePrivateShaders();
static void GenBuffers();
static void DeleteBuffers();
static void ClearBatches();
static void SwapBuffers();
static void RendererError(VivernaState& state,
                          [[maybe_unused]] std::string_view message);
static BatchId NewBatchInExistingBucket(Bucket& bucket);
static BatchId NewBatchInNewBucket(ShaderId shader);
static void SendDataToVbo(const RenderBatch& batch);
static void SendDataToEbo(const RenderBatch& batch);
static void BindTextures(const RenderBatch& batch);
static void PrepareDraw();
static void DepthPass();
static void DrawBatchIgnoreTextures(const RenderBatch& batch);
static void DrawBatch(const RenderBatch& batch);
static void DrawGlCommand(const GlDrawCommand& cmd);

void CheckForGLErrors(std::string_view origin) {
    GLenum glerr;
    while ((glerr = glGetError()) != GL_NO_ERROR) {
        std::string error_string;
        switch (glerr) {
            case GL_INVALID_ENUM:
                error_string = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error_string = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error_string = "GL_INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error_string = "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error_string = "GL_STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error_string = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error_string = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                error_string = std::to_string(glerr);
                break;
        }
        VERNA_LOGE(std::string(origin)
                   + " detected OpenGL error: " + error_string);
    }
}

void LoadPrivateShaders() {
    wireframe_shader = LoadShader("debug");
    depth_shader = LoadShader("depth");
}

void FreePrivateShaders() {
    FreeShader(wireframe_shader);
    FreeShader(depth_shader);
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
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(2);

    auto num_lights = RendererInfo::MaxPointLights();
    depth_fbos.resize(num_lights);
    depth_maps.resize(num_lights);
    glGenFramebuffers(num_lights, depth_fbos.data());
    glGenTextures(num_lights, depth_maps.data());
    for (int i = 0; i < num_lights; i++) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_maps[i]);
        for (auto j = 0; j < 6; j++) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0,
                         GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH,
                         SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                         nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                        GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, depth_fbos[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_maps[i],
                             0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CheckForGLErrors("GenBuffers");
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

void PrepareDraw() {
    const Scene& scene = Scene::GetActive();
    const Camera& cam = scene.GetCamera();
    frame_data.camera_data.projection_matrix = cam.GetProjectionMatrix();
    frame_data.camera_data.view_matrix = cam.GetViewMatrix();
    frame_data.camera_data.pv_matrix = frame_data.camera_data.projection_matrix
                                       * frame_data.camera_data.view_matrix;
    const auto& scene_lights = scene.PointLights();
    size_t num_lights =
        std::min(static_cast<size_t>(gpu::FrameData::MAX_POINT_LIGHTS),
                 scene_lights.size());
    frame_data.num_point_lights = static_cast<int32_t>(num_lights);
    for (size_t i = 0; i < num_lights; i++)
        frame_data.point_lights[i] = gpu::PointLightData(scene_lights[i]);

    constexpr auto shadow_aspect_ratio =
        static_cast<float>(SHADOW_MAP_WIDTH)
        / static_cast<float>(SHADOW_MAP_HEIGHT);
    frame_data.light_projection = Mat4f::Perspective(
        maths::Pi() * 0.5f, shadow_aspect_ratio, cam.near_plane, cam.far_plane);

    ubo::SendData(gpu::FrameData::BLOCK_BINDING, &frame_data);
}

void DepthPass() {
    constexpr float shadow_aspect_ratio = 1.0f;
    const auto& scene = Scene::GetActive();

    glUseProgram(depth_shader.id);
    auto num_lights =
        std::min(static_cast<size_t>(RendererInfo::MaxPointLights()),
                 scene.PointLights().size());
    for (size_t i = 0; i < num_lights; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, depth_fbos[i]);
        glClear(GL_DEPTH_BUFFER_BIT);
        static const auto loc_light_id =
            glGetUniformLocation(depth_shader.id, "light_id");
        glUniform1i(loc_light_id, static_cast<GLint>(i));
        ShaderId shader;
        Bucket bucket;
        for (size_t i = 0; i < shader_to_bucket.Size(); i++) {
            shader_to_bucket.Get(i, shader, bucket);
            for (BatchId batch_id : bucket) {
                const RenderBatch& batch = render_batches[batch_id];
                DrawBatchIgnoreTextures(batch);
            }
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void InitializeRenderer(VivernaState& state) {
    if (state.GetFlag(VivernaState::RENDERER_INITIALIZED_FLAG))
        return;
    if (!state.GetFlag(VivernaState::RENDERER_API_INITIALIZED_FLAG)) {
        RendererError(state,
                      "InitializeRendererAPI should be called before "
                      "InitializeRenderer!");
        return;
    }
    if (!state.GetFlag(VivernaState::ASSETS_INITIALIZED_FLAG)) {
        RendererError(state,
                      "InitializeAssets should be called before "
                      "InitializeRenderer!");
        return;
    }

    GenBuffers();
    LoadPrivateShaders();
    glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
    glClearDepthf(0.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GEQUAL);
    glDepthMask(GL_TRUE);
    glCullFace(GL_BACK);

    Scene::GetActive().Setup();

    state.SetFlag(VivernaState::RENDERER_INITIALIZED_FLAG, true);
    native_window = state.native_window;
    VERNA_LOGI("Renderer initialized!");
}

void TerminateRenderer(VivernaState& state) {
    if (!state.GetFlag(VivernaState::RENDERER_INITIALIZED_FLAG))
        return;

    FreePrivateShaders();
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

void Render(const Mesh& mesh,
            const Material& material,
            const Transform& transform,
            ShaderId shader_id) {
    Render(mesh, material, transform.GetMatrix(), shader_id);
}

void Render(const Model& model,
            const Mat4f& transform_matrix,
            ShaderId shader_id) {
    const auto& meshes = model.Meshes();
    const auto& materials = model.Materials();
    for (size_t i = 0; i < meshes.size(); i++)
        Render(meshes[i], materials[i], transform_matrix, shader_id);
}

void Render(const Model& model,
            const Transform& transform,
            ShaderId shader_id) {
    Render(model, transform.GetMatrix(), shader_id);
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

void DrawGlCommand(const GlDrawCommand& cmd) {
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

void DrawBatchIgnoreTextures(const RenderBatch& batch) {
    static std::array<const GLvoid*, RenderBatch::MAX_MESHES> indices_offsets;
    static std::array<GLint, RenderBatch::MAX_MESHES> vertices_offsets;

    SendDataToVbo(batch);
    SendDataToEbo(batch);
    ubo::SendData(gpu::DrawData::BLOCK_BINDING, &batch.draw_data);

    GlDrawCommand draw_command;
    batch.GenerateOffsets(indices_offsets, vertices_offsets);
    draw_command.basevertex = vertices_offsets.data();
    draw_command.drawcount = batch.num_meshes;
    draw_command.indices = indices_offsets.data();
    draw_command.count = batch.indices_count.data();
    DrawGlCommand(draw_command);
}

void DrawBatch(const RenderBatch& batch) {
    BindTextures(batch);
    DrawBatchIgnoreTextures(batch);
}

void Draw() {
    if (native_window == nullptr) {
        VERNA_LOGE(
            "Called Draw() before Renderer could access current context!");
        ClearBatches();
        return;
    }
#ifndef NDEBUG
    GLenum glerr;
    while ((glerr = glGetError()) != GL_NO_ERROR)
        VERNA_LOGE("Caught OpenGL error in Draw(): " + std::to_string(glerr));
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (shader_to_bucket.Empty()) {
        VERNA_LOGW("Nothing to draw!");
        SwapBuffers();
        return;
    }
    VERNA_LOGE_IF(render_batches.empty(),
                  "There are render buckets but no render batches!");

    PrepareDraw();
    DepthPass();

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
int MaxMaterialTextures() {
    auto total = MaxTextureUnits();
    return total == 16 ? 6 : std::min(32, total / 2);
}
int MaxPointLights() {
    return std::min(static_cast<int>(gpu::FrameData::MAX_POINT_LIGHTS),
                    MaxTextureUnits() - MaxMaterialTextures());
}
}  // namespace RendererInfo
}  // namespace verna