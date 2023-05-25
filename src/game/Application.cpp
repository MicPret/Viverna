#include <game/core/Application.hpp>
#include <game/CameraController.hpp>
#include <game/NewCube.hpp>
#include <game/systems/MeshRenderer.hpp>
#include <game/gui/EditorGui.hpp>

#include <viverna/core/Input.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Window.hpp>
#include <viverna/ecs/EntityName.hpp>
#include <viverna/ecs/World.hpp>

#include <vector>

namespace verna {

static Scene* scene;
static ShaderId shader;
static std::vector<Entity> renderables;
static int selected_id = -1;
TextureId diffuse;
TextureId specular;

void OnAppResume(VivernaState& app_state) {
    editor::InitGUI();
    scene = &Scene::GetActive();
    Camera& camera = scene->camera;
    camera.position = Vec3f(0.0f, 0.0f, -3.0f);
    DirectionLight& light = scene->direction_light;
    light.direction = Vec3f(0.2f, -0.8f, 0.4f).Normalized();

    shader = scene->shader_manager.LoadShader("blinn-phong");
    diffuse = scene->texture_manager.LoadTextureFromColor(
        Color4f(0.7f, 0.7f, 0.7f, 1.0f), {});
    specular = scene->texture_manager.LoadTextureFromColor(
        Color4f(0.3f, 0.3f, 0.3f, 1.0f), {});

    auto first_entity = editor::NewCube(scene->world);
    renderables.push_back(first_entity);
    scene->world.AddSystem(Family::From<Mesh, Material, Transform, ShaderId>(),
                           editor::Render);
}
void OnAppPause(VivernaState& app_state) {
    editor::TermGUI();
    scene->ReleaseResources();
}
void OnAppUpdate(VivernaState& app_state, DeltaTime<float, Seconds> dt) {
    NextFrame();

    KeyListener escape(Key::Escape);
    KeyListener space(Key::Space);
    if (escape.Pressed()) {
        app_state.SetFlag(VivernaState::RUNNING_FLAG, false);
        return;
    }

    scene->world.RunSystems(dt);

    if (selected_id >= 0) {
        static int old_id = -1;
        static Mesh::id_type old_mesh_id;
        static Transform old_transform;
        static BoundingBox bounds;
        Mesh mesh;
        Transform transform;
        scene->world.GetComponents(renderables[selected_id], mesh, transform);
        if (old_id != selected_id || !old_transform.IsAlmostEqual(transform)
            || old_mesh_id != mesh.id) {
            bounds.Recalculate(mesh, transform);
            old_id = selected_id;
            old_mesh_id = mesh.id;
            old_transform = transform;
        }
        Render(bounds);
    }

    static float camera_speed = 2.0f;
    bool space_pressed = space.Pressed();
    editor::UpdateCamera(scene->camera, camera_speed, dt.count(),
                         space_pressed);

    Draw();
    editor::BeginGUI();
    if (editor::BeginWindow("Viverna")) {
        if (editor::BeginTabs()) {
            editor::EntityTab(scene->world, renderables, selected_id);
            editor::AssetsTab(scene->world, renderables);
            editor::LightingTab(scene->direction_light);
            editor::CameraTab(camera_speed);
            editor::EndTabs();
        }
    }
    editor::EndWindow();
    editor::EndGUI();
}
}  // namespace verna
