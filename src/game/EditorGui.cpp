#include <game/gui/EditorGui.hpp>
#include <game/components/EntityName.hpp>

#include <viverna/core/Input.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/graphics/Window.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

#include <array>

namespace editor {

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
        names.reserve(entities.size());
        for (verna::Entity e : entities)
            names.push_back(world.GetComponent<editor::EntityName>(e).String());
        std::vector<const char*> labels;
        labels.resize(names.size());
        for (size_t i = 0; i < labels.size(); i++)
            labels[i] = names[i].c_str();

        // Entity list
        if (ImGui::BeginListBox("##entities")) {
            for (size_t i = 0; i < labels.size(); i++) {
                const bool is_selected = selected_id == static_cast<int>(i);
                if (ImGui::Selectable(labels[i], is_selected))
                    selected_id = static_cast<int>(i);
            }
            ImGui::EndListBox();
        }

        if (selected_id >= 0) {
            // Rename button
            ImGui::SameLine();
            static bool rename_pressed = false;
            if (ImGui::Button("Rename"))
                rename_pressed = true;
            if (rename_pressed) {
                static std::array<char, 256> buf{};
                ImGui::SameLine();
                if (ImGui::InputText("New name", buf.data(), buf.size(),
                                     ImGuiInputTextFlags_EnterReturnsTrue)) {
                    rename_pressed = false;
                    world.SetComponent<editor::EntityName>(
                        entities[selected_id], buf.data());
                }
            }

            // Transform
            verna::Vec3f vec;
            std::array<float, 3> arr;
            auto transform =
                world.GetComponent<verna::Transform>(entities[selected_id]);
            vec = transform.position;
            arr = {vec.x, vec.y, vec.z};
            ImGui::DragFloat3("Position", arr.data(), 0.01f, -100.0f, 100.0f);
            transform.position = verna::Vec3f(arr[0], arr[1], arr[2]);
            vec = transform.scale;
            arr = {vec.x, vec.y, vec.z};
            ImGui::DragFloat3("Scale", arr.data(), 0.01f, 0.01f, 100.0f);
            transform.scale = verna::Vec3f(arr[0], arr[1], arr[2]);
            world.SetComponent(entities[selected_id], transform);
        }
        // New Entity button
        if (ImGui::Button("New Entity")) {
            selected_id = entities.size();
            entities.push_back(on_new_entity(world));
        }
        ImGui::EndTabItem();
    }
}

void LightingTab() {
    if (!ImGui::BeginTabItem("Lighting"))
        return;

    verna::Vec3f vec;
    std::array<float, 3> arr;
    auto& light = verna::Scene::GetActive().GetDirectionLight();

    vec = light.direction;
    arr = {vec.x, vec.y, vec.z};
    ImGui::DragFloat3("Direction", arr.data(), 0.01, -1.0f, 1.0f);
    light.direction = verna::Vec3f(arr[0], arr[1], arr[2]).Normalized();

    vec = light.ambient;
    arr = {vec.x, vec.y, vec.z};
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    ImGui::ColorPicker3("Ambient", arr.data());
    light.ambient = verna::Vec3f(arr[0], arr[1], arr[2]);

    vec = light.diffuse;
    arr = {vec.x, vec.y, vec.z};
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    ImGui::ColorPicker3("Diffuse", arr.data());
    light.diffuse = verna::Vec3f(arr[0], arr[1], arr[2]);

    vec = light.specular;
    arr = {vec.x, vec.y, vec.z};
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    ImGui::ColorPicker3("Specular", arr.data());
    light.diffuse = verna::Vec3f(arr[0], arr[1], arr[2]);

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
}  // namespace editor
