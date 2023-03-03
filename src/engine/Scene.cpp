#include <viverna/core/Scene.hpp>
#include <viverna/graphics/gpu/FrameData.hpp>
#include <viverna/graphics/Window.hpp>
#include <viverna/maths/MathUtils.hpp>

namespace verna {

static Scene* active_scene = nullptr;

void Scene::Setup() {
    float width = static_cast<float>(WindowWidth());
    float height = static_cast<float>(WindowHeight());
    camera.aspect_ratio = width / height;
}

Scene& Scene::GetActive() {
    static Scene default_scene;
    if (active_scene == nullptr) {
        default_scene.Setup();
        active_scene = &default_scene;
    }
    return *active_scene;
}

void Scene::SetActive(Scene& scene) {
    active_scene = &scene;
}

}  // namespace verna