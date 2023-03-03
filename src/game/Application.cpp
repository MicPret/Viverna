#include <game/core/Application.hpp>

#include <viverna/core/Input.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Shader.hpp>

#include <game/core/Fruit.hpp>
#include <game/core/Snake.hpp>
#include <game/core/Walls.hpp>

namespace verna {

static constexpr float distance = 20.0f;
static snake::Snake snake(distance);
static snake::Fruit fruit(distance);
static snake::Walls walls(distance);
static Scene scene;
static ShaderId shader;
static KeyListener escape(Key::Escape);
static MouseListener mouse;
static void RenderScene();
static void Reset();
static void RecalculateFruit();

void OnAppResume(VivernaState& app_state) {
    scene.Setup();
    Scene::SetActive(scene);

    shader = LoadShader("blinn-phong");
    walls.Setup();
    snake.Setup();
    fruit.Setup();
    RecalculateFruit();

    auto& lights = scene.PointLights();
    PointLight pl;
    pl.diffuse = Vec3f(0.9f, 0.8f, 1.0f);
    pl.specular = Vec3f(1.0f, 1.0f, 1.0f);
    // pl.ambient = Vec3f(0.001f, 0.001f, 0.001f);
    pl.ambient = Vec3f();
    for (const auto& c : walls.Colliders()) {
        pl.position = c.Center() - (0.6f * c.Depth());
        lights.push_back(pl);
    }
}

void OnAppPause(VivernaState& app_state) {
    walls.Dispose();
    snake.Dispose();
    fruit.Dispose();
    FreeShader(shader);
}

void OnAppUpdate(VivernaState& app_state, DeltaTime<float, Seconds> dt) {
    if (escape.Pressed()) {
        app_state.SetFlag(VivernaState::RUNNING_FLAG, false);
        return;
    }
    const auto& camera = scene.GetCamera();
    unsigned int mouse_x, mouse_y;
    Vec3f target = mouse.Pressed(mouse_x, mouse_y)
                       ? camera.ToWorldCoords(mouse_x, mouse_y, distance)
                       : snake.HeadPosition() + snake.Direction();
    target.z = distance;
    snake.MoveTowards(dt.count(), target);
    snake::BufferView<BoundingBox> wall_colliders(walls.Colliders().data(),
                                                  walls.Colliders().size());

    if (snake.Hits(fruit.Collider())) {
        snake.GrowTail();
        RecalculateFruit();
    } else if (snake.HitsItself()
               || snake.Hits(snake::BufferView<BoundingBox>(
                   walls.Colliders().data(), walls.Colliders().size()))) {
        Reset();
    }

    RenderScene();
    Draw();
}

static void RenderScene() {
    snake.Render(shader);
    fruit.Render(shader);
    walls.Render(shader);
}

static void Reset() {
    scene.GetCamera().Reset();
    snake.Dispose();
    snake.Setup();
    RecalculateFruit();
}

static void RecalculateFruit() {
    snake::BufferView<BoundingSphere> snake_cols(snake.Colliders().data(),
                                                 snake.Colliders().size());
    snake::BufferView<BoundingBox> walls_cols(walls.Colliders().data(),
                                              walls.Colliders().size());
    Vec3f pos = scene.GetCamera().ToWorldCoords(0, 0, distance);
    Vec2f bounds_min(pos.x, -pos.y);
    Vec2f bounds_max(-pos.x, pos.y);
    fruit.Recalculate(snake_cols, walls_cols, bounds_min, bounds_max);
}

}  // namespace verna
