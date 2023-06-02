#include <game/core/Application.hpp>

#include <viverna/core/Input.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/graphics/DirectionLight.hpp>
#include <viverna/graphics/Renderer.hpp>
#include <viverna/graphics/Shader.hpp>

#include <game/core/CameraController.hpp>
#include <game/core/Fruit.hpp>
#include <game/core/Snake.hpp>
#include <game/core/Walls.hpp>

namespace verna {
static constexpr float distance = 20.0f;
static snake::Snake snake(distance);
static snake::Fruit fruit(distance);
static snake::Walls walls(distance);
static snake::CameraController camera_controller;
static ShaderId shader;
static KeyListener escape(Key::Escape);
static KeyListener space(Key::Space);
static MouseListener mouse;
static void RenderScene();
static void Reset();
static void RecalculateFruit();

void OnAppResume(VivernaState& app_state) {
    auto& scene = Scene::GetActive();
    scene.camera.SetAspectRatioFromWindowSize();

    shader = scene.shader_manager.LoadShader("blinn-phong");
    walls.Setup();
    snake.Setup();
    fruit.Setup();
    RecalculateFruit();

    DirectionLight light;
    light.direction = Vec3f(0.1f, -0.4f, 0.5f);
    light.ambient = Vec3f(0.01f);
    light.diffuse = Vec3f(0.9f);
    light.specular = Vec3f(0.3f);
    scene.direction_light = light;

    camera_controller.SetCamera(scene.camera);
}

void OnAppPause(VivernaState& app_state) {
    walls.Dispose();
    snake.Dispose();
    fruit.Dispose();
    Scene::GetActive().ReleaseResources();
}

void OnAppUpdate(VivernaState& app_state, DeltaTime<float, Seconds> dt) {
    NextFrame();

    if (escape.Pressed()) {
        app_state.SetFlag(VivernaState::RUNNING_FLAG, false);
        return;
    }
    auto& scene = Scene::GetActive();
    if (space.Pressed()) {
        auto& light = scene.direction_light;
        light.direction = Quaternion(Vec3f::UnitY(), dt.count() * 0.3f)
                              .Rotate(light.direction);
    }

    const auto& camera = scene.camera;
    unsigned int mouse_x, mouse_y;
    Vec3f target = mouse.Pressed(mouse_x, mouse_y)
                       ? camera.ToWorldCoords(mouse_x, mouse_y, distance)
                       : snake.HeadPosition() + snake.Direction();
    target.z = distance;
    snake.MoveTowards(dt.count(), target);
    snake::BufferView wall_colliders(walls.Colliders().data(),
                                     walls.Colliders().size());

    if (snake.Hits(fruit.Collider())) {
        snake.GrowTail();
        RecalculateFruit();
    } else if (snake.HitsItself()
               || snake.Hits(snake::BufferView(walls.Colliders().data(),
                                               walls.Colliders().size()))) {
        Reset();
    }
    camera_controller.Update(dt.count());

    RenderScene();
    Draw();
}

static void RenderScene() {
    snake.Render(shader);
    fruit.Render(shader);
    walls.Render(shader);
}

static void Reset() {
    snake.Restart();
    RecalculateFruit();
}

static void RecalculateFruit() {
    snake::BufferView snake_cols(snake.Colliders().data(),
                                 snake.Colliders().size());
    snake::BufferView walls_cols(walls.Colliders().data(),
                                 walls.Colliders().size());
    const auto& camera = Scene::GetActive().camera;
    Vec3f pos = camera.ToWorldCoords(0, 0, distance);
    Vec2f bounds_min(pos.x, -pos.y);
    Vec2f bounds_max(-pos.x, pos.y);
    fruit.Recalculate(snake_cols, walls_cols, bounds_min, bounds_max);
}

}  // namespace verna
