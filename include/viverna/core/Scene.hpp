#ifndef VERNA_SCENE_HPP
#define VERNA_SCENE_HPP

#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/DirectionLight.hpp>

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
    constexpr const DirectionLight& GetDirectionLight() const {
        return dir_light;
    }
    constexpr DirectionLight& GetDirectionLight() { return dir_light; }

   private:
    Camera camera;
    DirectionLight dir_light;
};
}  // namespace verna

#endif