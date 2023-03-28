#ifndef VERNA_GAME_CAMERA_CONTROLLER_HPP
#define VERNA_GAME_CAMERA_CONTROLLER_HPP

#include <viverna/graphics/Camera.hpp>

namespace snake {
class CameraController {
   public:
    constexpr CameraController() = default;
    constexpr void SetCamera(verna::Camera& cam) { camera = &cam; }
    void Update(float dt);

   private:
    verna::Camera* camera = nullptr;
};
}  // namespace snake

#endif