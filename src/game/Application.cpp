#include <game/core/Application.hpp>
#include <game/CameraController.hpp>

#include <viverna/core/Input.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Window.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace verna {

static void InitGUI();
static void TermGUI();
static void BeginGUI();
static void EndGUI();
static void SpawnCube(const Vec3f& position = Vec3f());

static Scene* scene;
static ShaderId shader;
static std::vector<Mesh> meshes;
static std::vector<Transform> transforms;
static std::vector<Material> materials;

void OnAppResume(VivernaState& app_state) {
    InitGUI();
    scene = &Scene::GetActive();
    Camera& camera = scene->GetCamera();
    camera.position = Vec3f(0.0f, 0.0f, -3.0f);
    shader = LoadShader("blinn-phong");
    SpawnCube();
}
void OnAppPause(VivernaState& app_state) {
    TermGUI();
    for (const auto& m : materials) {
        FreeTexture(m.textures[0]);
        FreeTexture(m.textures[1]);
    }
    FreeShader(shader);
}
void OnAppUpdate(VivernaState& app_state, DeltaTime<float, Seconds> dt) {
    NextFrame();

    KeyListener escape(Key::Escape);
    if (escape.Pressed()) {
        app_state.SetFlag(VivernaState::RUNNING_FLAG, false);
        return;
    }

    for (size_t i = 0; i < meshes.size(); i++)
        Render(meshes[i], materials[i], transforms[i], shader);

    static float camera_speed = 1.0f;
    editor::UpdateCamera(scene->GetCamera(), camera_speed, dt.count());

    Draw();
    BeginGUI();
    if (ImGui::Begin("Viverna")) {
        static std::array<float, 3> position;
        ImGui::SliderFloat3("Position", position.data(), -50.0f, 50.0f);
        if (ImGui::Button("New Cube")) {
            SpawnCube(Vec3f(position[0], position[1], position[2]));
        }
        ImGui::Spacing();
        ImGui::SliderFloat("Camera speed", &camera_speed, 1.0f, 10.0f);
    }
    ImGui::End();
    EndGUI();
}

static void InitGUI() {
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    if (WindowWidth() > 1920) {
        constexpr float scale = 2.5f;
        io.FontGlobalScale = scale;
        io.DisplayFramebufferScale = ImVec2(scale, scale);
    }
    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 460 core");
}
static void TermGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
static void BeginGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
static void EndGUI() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void SpawnCube(const Vec3f& position) {
    meshes.push_back(LoadPrimitiveMesh(PrimitiveMeshType::Cube));
    Transform t;
    t.position = position;
    transforms.push_back(t);
    Material m;
    m.textures[0] = LoadTextureFromColor(0.7f, 0.7f, 0.7f, 1.0f);
    m.textures[1] = LoadTextureFromColor(0.0f, 0.0f, 0.0f, 1.0f);
    materials.push_back(m);
}
}  // namespace verna
