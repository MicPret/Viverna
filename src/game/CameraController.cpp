#include <game/CameraController.hpp>
#include <viverna/core/Input.hpp>
#include <viverna/graphics/Window.hpp>

namespace editor {

void UpdateCamera(verna::Camera& camera,
                  float speed,
                  float dt,
                  bool enable_rotation) {
    verna::KeyListener w(verna::Key::W);
    verna::KeyListener a(verna::Key::A);
    verna::KeyListener s(verna::Key::S);
    verna::KeyListener d(verna::Key::D);

    if (enable_rotation) {
        verna::MouseListener mouse;
        unsigned mouse_x;
        unsigned mouse_y;
        mouse.Position(mouse_x, mouse_y);
        float dx = static_cast<float>(mouse_x)
                       / static_cast<float>(verna::WindowWidth()) * 2.0f
                   - 1.0f;
        float dy = static_cast<float>(mouse_y)
                       / static_cast<float>(verna::WindowHeight()) * 2.0f
                   - 1.0f;
        camera.rotation = verna::Quaternion(verna::Vec3f::UnitY(),
                                            dx * (2.0f * verna::maths::Pi()))
                          * verna::Quaternion(verna::Vec3f::UnitX(),
                                              dy * verna::maths::Pi() * 0.49f);
    }

    verna::Vec3f forward = camera.Forward();
    verna::Vec3f right = verna::Vec3f::UnitY().Cross(forward).Normalized();
    float z = static_cast<float>(w.Pressed()) - static_cast<float>(s.Pressed());
    float x = static_cast<float>(d.Pressed()) - static_cast<float>(a.Pressed());

    auto direction = z * forward + x * right;

    if (verna::maths::AreAlmostEqual(direction.SquaredMagnitude(), 0.0f, 1e-6))
        return;

    camera.position += (speed * dt) * direction.Normalized();
}

}  // namespace editor