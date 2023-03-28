#include <game/core/CameraController.hpp>

#include <viverna/core/Debug.hpp>
#include <viverna/core/Input.hpp>
#include <viverna/graphics/Window.hpp>

namespace snake {
void CameraController::Update(float dt) {
    if (camera == nullptr) {
        VERNA_LOGE("No camera attached to CameraController!");
        return;
    }

    constexpr float speed = 2.1f;
    static verna::KeyListener w(verna::Key::W);
    static verna::KeyListener a(verna::Key::A);
    static verna::KeyListener s(verna::Key::S);
    static verna::KeyListener d(verna::Key::D);
    static verna::KeyListener p(verna::Key::P);
    static verna::MouseListener mouse;

    if (!p.Pressed())
        return;
    unsigned int x;
    unsigned int y;
    mouse.Position(x, y);
    auto mouse_01 = verna::Vec2f(static_cast<float>(x) / verna::WindowWidth(),
                                 static_cast<float>(y) / verna::WindowHeight());
    auto rot =
        verna::Vec2f((mouse_01.y * 2.0f - 1.0f) * (verna::maths::Pi() * 0.5f),
                     (mouse_01.x * 2.0f - 1.0f) * verna::maths::Pi());

    auto cam_forward = camera->Forward();
    auto cam_right = verna::Vec3f::UnitY().Cross(cam_forward).Normalized();
    camera->rotation = verna::Quaternion(verna::Vec3f::UnitY(), rot.y)
                       * verna::Quaternion(cam_right, rot.x);

    float forward =
        static_cast<float>(w.Pressed()) - static_cast<float>(s.Pressed());
    float right =
        static_cast<float>(d.Pressed()) - static_cast<float>(a.Pressed());

    auto translation = forward * cam_forward + right * cam_right;
    camera->position += speed * dt * translation;
}
}  // namespace snake