#include <game/core/Application.hpp>
#include <game/CameraController.hpp>
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
    world.ClearAll();
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

    if (selected_id >= 0) {
        Mesh mesh;
        Transform transform;
        world.GetComponents(renderables[selected_id], mesh, transform);
        auto bounds = mesh.bounds;
        // TODO optimize
        bounds.Recalculate(mesh, transform);
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
            auto entity_generator = [](World& world_,
                                       std::vector<Entity>& entities_) {
                Entity e = NewRenderable(
                    world_,
                    verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Cube));
                entities_.push_back(e);
                return e;
            };
            editor::EntityTab(world, renderables, selected_id,
                              entity_generator);
            editor::AssetsTab(world, renderables, entity_generator);
            editor::LightingTab(scene->GetDirectionLight());
            editor::CameraTab(camera_speed);
            editor::EndTabs();
        }
    }
    editor::EndWindow();
    editor::EndGUI();
}

Entity NewRenderable(World& world_, Mesh&& mesh) {
    static TextureId diffuse = LoadTextureFromColor(0.7f, 0.7f, 0.7f, 1.0f);
    static TextureId specular = LoadTextureFromColor(0.3f, 0.3f, 0.3f, 1.0f);
    Material mat;
    mat.textures[Material::DIFFUSE_INDEX] = diffuse;
    mat.textures[Material::SPECULAR_INDEX] = specular;
    mat.parameters[Material::SHININESS_INDEX] = 20.06f;
    EntityName name;
    name.str = "New entity";

    Entity e =
        world_.NewEntity<Mesh, Material, Transform, ShaderId, EntityName>();
    world_.SetComponents(e, mesh, mat, Transform(), shader, name);
    return e;
}
}  // namespace verna
