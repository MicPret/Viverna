#include <viverna/core/Debug.hpp>
#include <viverna/graphics/GraphicsAPIHelper.hpp>
#include <viverna/graphics/NativeWindow.hpp>
#include <viverna/graphics/Renderer.hpp>


namespace verna {

static GLuint vao, vbo, ebo;
static bool initialized = false;

static void RenderNaive(const Mesh& mesh,
                        const Mat4f& transform_matrix,
                        ShaderId shader_id);
static void DrawNaive();

void InitializeRenderer() {
    if (initialized)
        return;
    if (!InitGraphicsAPI()) {
        VERNA_LOGE("InitGraphicsAPI() failed!");
        return;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // TODO 4096 is random
    glBufferData(GL_ARRAY_BUFFER, 4096, nullptr, GL_DYNAMIC_DRAW);
    // glBufferStorage(GL_ARRAY_BUFFER, 4096, nullptr, GL_DYNAMIC_STORAGE_BIT);
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4096, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, texture_coords)));
    glEnableVertexAttribArray(1);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    initialized = true;
}

void TerminateRenderer() {
    if (!initialized)
        return;

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    initialized = false;

    TermGraphicsAPI();
}

void Render(const Mesh& mesh,
            const Mat4f& transform_matrix,
            ShaderId shader_id) {
    VERNA_ASSERT(initialized);
    RenderNaive(mesh, transform_matrix, shader_id);
}

void Draw() {
    VERNA_ASSERT(initialized);
    DrawNaive();
}

static void RenderNaive(const Mesh& mesh,
                        const Mat4f& transform_matrix,
                        ShaderId shader_id) {
    [[maybe_unused]] static int initializer = []() {
        VERNA_LOGW("Using RenderNaive(...)");
        return 0;
    }();
    glUseProgram(shader_id);
    GLint loc = glGetUniformLocation(shader_id, "transform_matrix");
    if (loc == -1)
        VERNA_LOGE(
            "RenderNaive(...): there is no uniform named 'transform_matrix'!");
    else
        glUniformMatrix4fv(loc, 1, GL_FALSE, transform_matrix.raw.data());
    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.vertices.size() * sizeof(Vertex),
                    mesh.vertices.data());
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    mesh.indices.size() * sizeof(uint32_t),
                    mesh.indices.data());
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
}

static void DrawNaive() {
    SwapNativeWindowBuffers();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

}  // namespace verna