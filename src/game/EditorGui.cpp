#include <game/gui/EditorGui.hpp>
#include <game/components/EntityName.hpp>

#include <viverna/core/Assets.hpp>
#include <viverna/core/Input.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/data/SparseSet.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Window.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <array>

namespace editor {

static void RenameButton(verna::World& world, verna::Entity entity);
static void TransformGUI(verna::World& world, verna::Entity entity);
static void MaterialGUI(verna::World& world, verna::Entity entity);

void InitGUI() {
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    if (verna::WindowWidth() > 1920) {
        constexpr float scale = 2.5f;
        io.FontGlobalScale = scale;
        io.DisplayFramebufferScale = ImVec2(scale, scale);
    }
    ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
    ImGui_ImplOpenGL3_Init("#version 460 core");
}
void TermGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
void BeginGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void EndGUI() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
bool BeginWindow(const char* name) {
    return ImGui::Begin(name, nullptr, ImGuiWindowFlags_AlwaysAutoResize);
}
void EndWindow() {
    ImGui::End();
}

bool BeginTabs() {
    return ImGui::BeginTabBar("##tabs");
}
void EndTabs() {
    ImGui::EndTabBar();
}

void EntityTab(verna::World& world,
               std::vector<verna::Entity>& entities,
               int& selected_id,
               NewEntityFunc on_new_entity) {
    if (ImGui::BeginTabItem("Entities")) {
        // Get entities' names
        std::vector<std::string> names;
        std::vector<const char*> labels;
        auto entities_size = entities.size();
        names.resize(entities_size);
        labels.resize(entities_size);
        for (size_t i = 0; i < entities_size; i++) {
            names[i] =
                world.GetComponent<editor::EntityName>(entities[i]).String();
            labels[i] = names[i].c_str();
        }
        // Entity list
        ImGui::ListBox("##entities", &selected_id, labels.data(),
                       labels.size());
        // Rename + Transform
        if (selected_id >= 0) {
            ImGui::SameLine();
            auto e = entities[selected_id];
            RenameButton(world, e);
            TransformGUI(world, e);
        }
        // New Entity button
        if (ImGui::Button("New Entity")) {
            selected_id = static_cast<int>(entities_size);
            entities.push_back(on_new_entity(world));
        }
        // Material
        if (selected_id >= 0) {
            auto e = entities[selected_id];
            MaterialGUI(world, e);
        }
        ImGui::EndTabItem();
    }
}

void LightingTab(verna::DirectionLight& dirlight) {
    if (!ImGui::BeginTabItem("Lighting"))
        return;

    ImGui::DragFloat3("Direction", &dirlight.direction.x, 0.01f, -1.0f, 1.0f);
    dirlight.direction = dirlight.direction.Normalized();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    ImGui::ColorPicker3("Ambient", &dirlight.ambient.x);

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    ImGui::ColorPicker3("Diffuse", &dirlight.diffuse.x);

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    ImGui::ColorPicker3("Specular", &dirlight.specular.x);

    ImGui::EndTabItem();
}

void CameraTab(float& camera_speed) {
    if (!ImGui::BeginTabItem("Camera"))
        return;
    verna::KeyListener space(verna::Key::Space);
    bool space_pressed = space.Pressed();
    ImGui::Checkbox("Press space to enable camera rotation", &space_pressed);
    ImGui::SliderFloat("Camera speed", &camera_speed, 1.0f, 10.0f);
    ImGui::EndTabItem();
}

void AssetsTab() {
    if (!ImGui::BeginTabItem("Assets"))
        return;
    auto dirs = verna::GetAssetsInDirectory("meshes");
    ImGui::SeparatorText("Meshes");
    static int last_selected_mesh = -1;
    bool is_selected;
    for (size_t i = 0; i < dirs.size(); i++) {
        is_selected = (last_selected_mesh == static_cast<int>(i));
        const char* label = dirs[i].string().c_str();
        if (ImGui::Selectable(label, is_selected))
            last_selected_mesh = static_cast<int>(i);
    }
    dirs = verna::GetAssetsInDirectory("shaders");
    ImGui::SeparatorText("Shaders");
    static int last_selected_shader = -1;
    for (size_t i = 0; i < dirs.size(); i++) {
        is_selected = (last_selected_shader == static_cast<int>(i));
        const char* label = dirs[i].string().c_str();
        if (ImGui::Selectable(label, is_selected))
            last_selected_shader = static_cast<int>(i);
    }
    dirs = verna::GetAssetsInDirectory("textures");
    ImGui::SeparatorText("Textures");
    static int last_selected_texture = -1;
    for (size_t i = 0; i < dirs.size(); i++) {
        is_selected = (last_selected_texture == static_cast<int>(i));
        const char* label = dirs[i].string().c_str();
        if (ImGui::Selectable(label, is_selected))
            last_selected_texture = static_cast<int>(i);
    }
    ImGui::EndTabItem();
}

// local funcs
void RenameButton(verna::World& world, verna::Entity entity) {
    static bool rename_pressed = false;
    if (ImGui::Button("Rename"))
        rename_pressed = true;
    if (rename_pressed) {
        static std::array<char, 256> buf{};
        ImGui::SameLine();
        if (ImGui::InputText("New name", buf.data(), buf.size() - 1,
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
            rename_pressed = false;
            world.SetComponent(entity, editor::EntityName(buf.data()));
        }
    }
}

void TransformGUI(verna::World& world, verna::Entity entity) {
    auto transform = world.GetComponent<verna::Transform>(entity);
    ImGui::DragFloat3("Position", &transform.position.x, 0.01f, -100.0f,
                      100.0f);
    ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f, 0.01f, 100.0f);
    world.SetComponent(entity, transform);
}

void MaterialGUI(verna::World& world, verna::Entity entity) {
    auto material = world.GetComponent<verna::Material>(entity);

    std::string s;
    std::string label;
    for (size_t i = 0; i < material.textures.size(); i++) {
        auto texture = material.textures[i];
        if (!texture.IsValid())
            continue;
        label = "Texture #" + std::to_string(i) + ": ";
        auto path = verna::GetTexturePath(texture);
        label += path.empty() ? std::to_string(texture.id) : path.string();

        ImGui::TextUnformatted(label.c_str());
        ImGui::SameLine();
        ImGui::Image(reinterpret_cast<ImTextureID>(texture.id),
                     ImVec2(64.0f, 64.0f));
    }
    bool modified = false;
    for (size_t i = 0; i < material.parameters.size(); i++) {
        label = "Parameter #" + std::to_string(i);
        modified |= ImGui::DragFloat(label.c_str(), &material.parameters[i],
                                     0.01f, -100.0f, 100.0f);
    }
    if (modified)
        world.SetComponent(entity, material);
}
}  // namespace editor
