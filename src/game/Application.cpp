#include <game/core/Application.hpp>
#include <game/CameraController.hpp>
#include <game/systems/MeshRenderer.hpp>

#include <viverna/core/Input.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Window.hpp>
#include <viverna/ecs/World.hpp>

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
static Entity SpawnCube(const Vec3f& position = Vec3f());

static Scene* scene;
static World world;
static ShaderId shader;
static Entity last_cube;

void OnAppResume(VivernaState& app_state) {
    InitGUI();
    scene = &Scene::GetActive();
    Camera& camera = scene->GetCamera();
    camera.position = Vec3f(0.0f, 0.0f, -3.0f);
    DirectionLight& light = scene->GetDirectionLight();
    light.direction = Vec3f(0.2f, -0.8f, 0.4f).Normalized();
    shader = LoadShader("blinn-phong");
    last_cube = SpawnCube();
    world.AddSystem(Family::From<Mesh, Material, Transform, ShaderId>(),
                    editor::Render);
}
void OnAppPause(VivernaState& app_state) {
    TermGUI();
    auto& materials = world.GetComponentArray<Material>();
    for (const Material& m : materials)
        for (TextureId t : m.textures)
            FreeTexture(t);
    FreeShader(shader);
    world.Clear();
}
void OnAppUpdate(VivernaState& app_state, DeltaTime<float, Seconds> dt) {
    NextFrame();

    KeyListener escape(Key::Escape);
    KeyListener space(Key::Space);
    if (escape.Pressed()) {
        app_state.SetFlag(VivernaState::RUNNING_FLAG, false);
        return;
    }

    world.RunSystems(dt);

    static float camera_speed = 2.0f;
    bool space_pressed = space.Pressed();
    editor::UpdateCamera(scene->GetCamera(), camera_speed, dt.count(),
                         space_pressed);

    Draw();
    BeginGUI();
    Transform selected = world.GetComponent<Transform>(last_cube);
    float* vec3 = nullptr;
    if (ImGui::Begin("Viverna", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::BeginTabBar("TabBar")) {
            if (ImGui::BeginTabItem("Game objects")) {
                vec3 = reinterpret_cast<float*>(&selected.position);
                ImGui::DragFloat3("Position", vec3, 0.01f, -100.0f, 100.0f);
                vec3 = reinterpret_cast<float*>(&selected.scale);
                ImGui::DragFloat3("Scale", vec3, 0.01f, 0.01f, 100.0f);
                world.SetComponent(last_cube, selected);
                if (ImGui::Button("New Cube"))
                    last_cube = SpawnCube(selected.position);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Lighting")) {
                DirectionLight& light = scene->GetDirectionLight();
                vec3 = reinterpret_cast<float*>(&light.direction);
                ImGui::DragFloat3("Direction", vec3, 0.01, -1.0f, 1.0f);
                light.direction = light.direction.Normalized();
                vec3 = reinterpret_cast<float*>(&light.ambient);
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
                ImGui::ColorPicker3("Ambient", vec3);
                vec3 = reinterpret_cast<float*>(&light.diffuse);
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
                ImGui::ColorPicker3("Diffuse", vec3);
                vec3 = reinterpret_cast<float*>(&light.specular);
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
                ImGui::ColorPicker3("Specular", vec3);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Camera")) {
                ImGui::Checkbox("Press space to enable camera rotation",
                                &space_pressed);
                ImGui::SliderFloat("Camera speed", &camera_speed, 1.0f, 10.0f);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
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

static Entity SpawnCube(const Vec3f& position) {
    Material m;
    m.textures[Material::DIFFUSE_INDEX] =
        LoadTextureFromColor(0.7f, 0.7f, 0.7f, 1.0f);
    m.textures[Material::SPECULAR_INDEX] =
        LoadTextureFromColor(0.01f, 0.01f, 0.01f, 1.0f);

    Entity cube = world.NewEntity<Mesh, Material, Transform, ShaderId>();
    world.SetComponents(cube, LoadPrimitiveMesh(PrimitiveMeshType::Cube),
                        Transform(), m, shader);
    return cube;
}
}  // namespace verna
