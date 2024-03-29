#include <viverna/graphics/Renderer.hpp>
#include <viverna/core/BoundingBox.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/ShaderManager.hpp>
#include <viverna/graphics/Texture.hpp>
#include <viverna/graphics/Vertex.hpp>
#include <viverna/graphics/Window.hpp>
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

GLuint vao, vbo, ebo;
GLsizeiptr vbo_size, ebo_size;

gpu::FrameData frame_data;

ShaderManager shaders;
ShaderId wireframe_shader;
ShaderId dirlight_shader;
GLuint dirlight_depthmap;
GLuint dirlight_fbo;
constexpr GLsizei SHADOW_MAP_SIZE = 2048;
constexpr GLsizei SHADOW_MAP_WIDTH = SHADOW_MAP_SIZE;
constexpr GLsizei SHADOW_MAP_HEIGHT = SHADOW_MAP_SIZE;

BoundingBox render_bounds;
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
static void InitLights();
static void TermLights();
static void ResetRenderBounds();

static auto DirectionLightTUIndex() {
    return RendererInfo::MaxMaterialTextures();
}

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

void InitLights() {
    glGenFramebuffers(1, &dirlight_fbo);

    glGenTextures(1, &dirlight_depthmap);
    glActiveTexture(GL_TEXTURE0 + DirectionLightTUIndex());
    glBindTexture(GL_TEXTURE_2D, dirlight_depthmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH,
                 SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,
                 nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    constexpr std::array borderColor = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
                     borderColor.data());

    glBindFramebuffer(GL_FRAMEBUFFER, dirlight_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                           dirlight_depthmap, 0);
    constexpr GLenum none = GL_NONE;
    glDrawBuffers(1, &none);
    glReadBuffer(GL_NONE);
    [[maybe_unused]] auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    VERNA_LOGE_IF(
        status != GL_FRAMEBUFFER_COMPLETE,
        "Incomplete fbo for direction light: " + std::to_string(status));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TermLights() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteTextures(1, &dirlight_depthmap);
    glDeleteFramebuffers(1, &dirlight_fbo);
}

void LoadPrivateShaders() {
    wireframe_shader = shaders.LoadShader("wireframe");
    dirlight_shader = shaders.LoadShader("dirlight_depth");
}

void FreePrivateShaders() {
    shaders.FreeShader(wireframe_shader);
    shaders.FreeShader(dirlight_shader);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    if (vbo_bytes > vbo_size) {
        vbo_size = std::max(vbo_size * 3 / 2, vbo_bytes);
        glBufferData(GL_ARRAY_BUFFER, vbo_size, nullptr, GL_DYNAMIC_DRAW);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, vbo_bytes, batch.vertices.data());
}

void SendDataToEbo(const RenderBatch& batch) {
    const GLsizeiptr ebo_bytes = static_cast<GLsizeiptr>(
        batch.indices.size() * sizeof(decltype(RenderBatch::indices[0])));
    if (ebo_bytes > ebo_size) {
        ebo_size = std::max(ebo_size * 3 / 2, ebo_bytes);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_size, nullptr,
                     GL_DYNAMIC_DRAW);
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, ebo_bytes,
                    batch.indices.data());
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
    const DirectionLight& dirlight = scene.direction_light;

    Vec3f lightdir = dirlight.direction.Normalized();
    Mat4f view = Mat4f::LookAt(-lightdir, Vec3f(),
                               lightdir.Cross(Vec3f::UnitX()).Normalized());

    Vec3f min = render_bounds.MinPosition();
    Vec3f max = render_bounds.MaxPosition();
    std::array lightview = {min,
                            Vec3f(min.x, min.y, max.z),
                            Vec3f(min.x, max.y, min.z),
                            Vec3f(min.x, max.y, max.z),
                            Vec3f(max.x, min.y, min.z),
                            Vec3f(max.x, min.y, max.z),
                            Vec3f(max.x, max.y, min.z),
                            max};
    for (Vec3f& p : lightview) {
        Vec4f temp = view * Vec4f(p, 1.0f);
        p = (1.0f / temp.w) * temp.Xyz();
    }
    min = lightview[0];
    max = min;
    for (size_t i = 1; i < lightview.size(); i++) {
        min = Vec3f::Min(min, lightview[i]);
        max = Vec3f::Max(max, lightview[i]);
    }
    Mat4f proj = Mat4f::Ortho(min.x, max.x, max.y, min.y, min.z, max.z);

    frame_data.direction_light.ambient = Vec4f(dirlight.ambient, 0.0f);
    frame_data.direction_light.diffuse = Vec4f(dirlight.diffuse, 0.0f);
    frame_data.direction_light.specular = Vec4f(dirlight.specular, 0.0f);
    frame_data.direction_light.direction = Vec4f(dirlight.direction, 0.0f);
    frame_data.direction_light.pv_matrix = proj * view;
    frame_data.camera_data = gpu::CameraData(scene.camera);

    ubo::SendData(gpu::FrameData::BLOCK_BINDING, &frame_data);
}

void DepthPass() {
    glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, dirlight_fbo);
    glDepthFunc(GL_LESS);
    glCullFace(GL_FRONT);
    glClearDepthf(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0 + DirectionLightTUIndex());
    glBindTexture(GL_TEXTURE_2D, dirlight_depthmap);

    glUseProgram(dirlight_shader.id);
    ShaderId dontcare;
    Bucket bucket;
    for (size_t i = 0; i < shader_to_bucket.Size(); i++) {
        shader_to_bucket.Get(i, dontcare, bucket);
        for (BatchId batch_id : bucket) {
            const RenderBatch& batch = render_batches[batch_id];
            DrawBatchIgnoreTextures(batch);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDepthFunc(GL_GEQUAL);
    glClearDepthf(0.0f);
    glCullFace(GL_BACK);
    glViewport(0, 0, WindowWidth(), WindowHeight());
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

    InitLights();

    state.SetFlag(VivernaState::RENDERER_INITIALIZED_FLAG, true);
    native_window = state.native_window;
    VERNA_LOGI("Renderer initialized!");
}

void TerminateRenderer(VivernaState& state) {
    if (!state.GetFlag(VivernaState::RENDERER_INITIALIZED_FLAG))
        return;

    TermLights();
    FreePrivateShaders();
    DeleteBuffers();

    state.SetFlag(VivernaState::RENDERER_INITIALIZED_FLAG, false);
    native_window = nullptr;
    VERNA_LOGI("Renderer terminated!");
}

static void Render(const Mesh& mesh,
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
    BoundingBox box = mesh.bounds;
    box.ApplyTransform(transform);
    if (render_bounds.Size().SquaredMagnitude() > 0.0f)
        render_bounds.Encapsulate(box);
    else
        render_bounds = box;
    Render(mesh, material, transform.GetMatrix(), shader_id);
}

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
        return;
    }
#ifndef NDEBUG
    [[maybe_unused]] GLenum glerr;
    while ((glerr = glGetError()) != GL_NO_ERROR)
        VERNA_LOGE("Caught OpenGL error in Draw(): " + std::to_string(glerr));
#endif
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (shader_to_bucket.Empty()) {
#ifdef VERNA_WARN_EMPTY_SCENE
        VERNA_LOGW("Nothing to draw!");
#endif
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
}

void NextFrame() {
    SwapBuffers();
    ClearBatches();
    ResetRenderBounds();
}

void Render(const BoundingBox& box) {
    Transform t;
    t.position = box.Center();
    t.scale = box.Size();
    Material mat;
    mat.SetCastsShadow(false);
    Mesh cube = LoadPrimitiveMesh(PrimitiveMeshType::Cube);
    Render(cube, mat, t, wireframe_shader);
}

void Render(const BoundingSphere& sphere) {
    Transform t;
    t.position = sphere.Position();
    t.scale = Vec3f(sphere.Radius());
    Material mat;
    mat.SetCastsShadow(false);
    Mesh mesh = LoadPrimitiveMesh(PrimitiveMeshType::Sphere);
    Render(mesh, mat, t, wireframe_shader);
}

void ResetRenderBounds() {
    render_bounds = BoundingBox();
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
}  // namespace RendererInfo
}  // namespace verna