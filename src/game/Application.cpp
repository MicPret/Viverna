#include <game/core/Application.hpp>
#include <game/CameraController.hpp>
#include <game/components/EntityName.hpp>
#include <game/systems/MeshRenderer.hpp>
#include <game/gui/EditorGui.hpp>

#include <viverna/core/Input.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Window.hpp>
#include <viverna/ecs/World.hpp>

#include <vector>

namespace verna {

static verna::Entity NewRenderable(World& world_, Mesh&& mesh);

static Scene* scene;
static World world;
static ShaderId shader;
static std::vector<Entity> renderables;
static int selected_id = -1;

void OnAppResume(VivernaState& app_state) {
    editor::InitGUI();
    scene = &Scene::GetActive();
    Camera& camera = scene->GetCamera();
    camera.position = Vec3f(0.0f, 0.0f, -3.0f);
    DirectionLight& light = scene->GetDirectionLight();
    light.direction = Vec3f(0.2f, -0.8f, 0.4f).Normalized();
    shader = LoadShader("blinn-phong");
    renderables.push_back(NewRenderable(
        world, verna::LoadPrimitiveMesh(PrimitiveMeshType::Cube)));
    world.AddSystem(Family::From<Mesh, Material, Transform, ShaderId>(),
                    editor::Render);
}
void OnAppPause(VivernaState& app_state) {
    editor::TermGUI();
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

    Transform target_transform;
    Mesh target_mesh;
    if (selected_id >= 0) {
        world.GetComponents(renderables[selected_id], target_transform,
                            target_mesh);
        auto bounds = target_mesh.bounds;
        bounds.ApplyTransform(target_transform);
        Render(bounds);
    }

    static float camera_speed = 2.0f;
    bool space_pressed = space.Pressed();
    editor::UpdateCamera(scene->GetCamera(), camera_speed, dt.count(),
                         space_pressed);

    Draw();
    editor::BeginGUI();
    if (editor::BeginWindow("Viverna")) {
        if (editor::BeginTabs()) {
            editor::EntityTab(
                world, renderables, selected_id, [](World& world_) {
                    return NewRenderable(world_,
                                         verna::LoadPrimitiveMesh(
                                             verna::PrimitiveMeshType::Cube));
                });
            editor::LightingTab();
            editor::CameraTab(camera_speed);
            editor::EndTabs();
        }
    }
    editor::EndWindow();
    editor::EndGUI();
}

verna::Entity NewRenderable(World& world_, Mesh&& mesh) {
    Material mat;
    mat.textures[Material::DIFFUSE_INDEX] =
        LoadTextureFromColor(0.7f, 0.7f, 0.7f, 1.0f);
    mat.textures[Material::SPECULAR_INDEX] =
        LoadTextureFromColor(0.3f, 0.3f, 0.3f, 1.0f);
    mat.parameters[Material::SHININESS_INDEX] = 20.06f;
    editor::EntityName name = "New entity";

    Entity e = world_.NewEntity<Mesh, Material, Transform, ShaderId,
                                editor::EntityName>();
    world_.SetComponents(e, mesh, mat, Transform(), shader, name);
    return e;
}
}  // namespace verna
