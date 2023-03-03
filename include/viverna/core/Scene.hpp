#ifndef VERNA_SCENE_HPP
#define VERNA_SCENE_HPP

#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/PointLight.hpp>

#include <vector>

namespace verna {
class Scene {
   public:
    Scene() = default;
    void Setup();
    static void SetActive(Scene& scene);
    static Scene& GetActive();
    constexpr const Camera& GetCamera() const { return camera; }
    constexpr Camera& GetCamera() { return camera; }
    constexpr const auto& PointLights() const { return point_lights; }
    constexpr auto& PointLights() { return point_lights; }

   private:
    Camera camera;
    std::vector<PointLight> point_lights;
};
}  // namespace verna

#endif