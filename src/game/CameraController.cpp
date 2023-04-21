#include <game/CameraController.hpp>

#include <viverna/core/Input.hpp>

namespace editor {

void UpdateCamera(verna::Camera& camera, float speed, float dt) {
    verna::KeyListener w(verna::Key::W);
    verna::KeyListener a(verna::Key::A);
    verna::KeyListener s(verna::Key::S);
    verna::KeyListener d(verna::Key::D);

    auto forward = camera.Forward();
    auto right = verna::Vec3f::UnitY().Cross(forward).Normalized();
    float z = static_cast<float>(w.Pressed()) - static_cast<float>(s.Pressed());
    float x = static_cast<float>(d.Pressed()) - static_cast<float>(a.Pressed());

    auto direction = z * forward + x * right;

    if (verna::maths::AreAlmostEqual(direction.SquaredMagnitude(), 0.0f, 1e-6))
        return;

    camera.position += (dt * speed) * direction.Normalized();
}

}  // namespace editor