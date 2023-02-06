#include "test_spinning_cube.hpp"
#include "test_interpolation.hpp"
#include "test_click.hpp"
#include "test_camera_movement.hpp"
#include "test_collision.hpp"

int main() {
    SpinningCube(3);
    Interpolation(4);
    Click(5);
    CameraMovement(6);
    Collision(8);
    return 0;
}