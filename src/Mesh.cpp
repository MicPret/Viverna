#include <viverna/graphics/Mesh.hpp>
#include <viverna/core/Debug.hpp>

namespace verna {

static Mesh LoadPrimitiveCube();
static Mesh LoadPrimitivePyramid();

Mesh LoadPrimitiveMesh(PrimitiveMeshType type) {
    switch (type) {
        case PrimitiveMeshType::Cube:
            return LoadPrimitiveCube();
        case PrimitiveMeshType::Pyramid:
            return LoadPrimitivePyramid();
        default:
            VERNA_LOGE(
                "LoadPrimitiveMesh failed: invalid primitive mesh type!");
            return Mesh();
    }
}

static Mesh LoadPrimitiveCube() {
    Mesh output;
    output.vertices.resize(8);
    output.vertices[0].position =
        Vec3f(-0.5f, -0.5f, -0.5f);  // btm left, front
    output.vertices[0].texture_coords = Vec2f(0.0f, 0.0f);
    output.vertices[1].position =
        Vec3f(0.5f, -0.5f, -0.5f);  // btm right, front
    output.vertices[1].texture_coords = Vec2f(1.0f, 0.0f);
    output.vertices[2].position = Vec3f(0.5f, 0.5f, -0.5f);  // top right, front
    output.vertices[2].texture_coords = Vec2f(1.0f, 1.0f);
    output.vertices[3].position = Vec3f(-0.5f, 0.5f, -0.5f);  // top left, front
    output.vertices[3].texture_coords = Vec2f(0.0f, 1.0f);
    output.vertices[4].position = Vec3f(-0.5f, 0.5f, 0.5f);  // top left, back
    output.vertices[4].texture_coords = Vec2f(1.0f, 1.0f);
    output.vertices[5].position = Vec3f(-0.5f, -0.5f, 0.5f);  // btm left, back
    output.vertices[5].texture_coords = Vec2f(1.0f, 0.0f);
    output.vertices[6].position = Vec3f(0.5f, -0.5f, 0.5f);  // btm right, back
    output.vertices[6].texture_coords = Vec2f(0.0f, 0.0f);
    output.vertices[7].position = Vec3f(0.5f, 0.5f, 0.5f);  // top right, back
    output.vertices[7].texture_coords = Vec2f(0.0f, 1.0f);
    output.indices = {
        0, 1, 2, 2, 3, 0,  // front
        5, 0, 3, 3, 4, 5,  // left
        4, 7, 6, 6, 5, 4,  // back
        7, 2, 6, 2, 1, 6,  // right
        2, 4, 3, 2, 7, 4,  // top
        5, 1, 0, 5, 6, 1   // bottom
    };
    return output;
}

static Mesh LoadPrimitivePyramid() {
    Mesh output;
    output.vertices.resize(5);
    output.vertices[0].position =
        Vec3f(-0.5f, -0.5f, -0.5f);  // btm left, front
    output.vertices[0].texture_coords = Vec2f(1.0f, 0.0f);
    output.vertices[1].position =
        Vec3f(0.5f, -0.5f, -0.5f);  // btm right, front
    output.vertices[1].texture_coords = Vec2f(0.0f, 0.0f);
    output.vertices[2].position = Vec3f(0.0f, 0.5f, 0.0f);  // top
    output.vertices[2].texture_coords = Vec2f(0.5f, 1.0f);
    output.vertices[3].position = Vec3f(-0.5f, -0.5f, 0.5f);  // btm left, back
    output.vertices[3].texture_coords = Vec2f(1.0f, 1.0f);
    output.vertices[4].position = Vec3f(0.5f, -0.5f, 0.5f);  // btm right, back
    output.vertices[4].texture_coords = Vec2f(0.0f, 1.0f);
    output.indices = {
        1, 0, 3, 3, 4, 1,  // bottom
        0, 1, 2,           // front
        3, 0, 2,           // left
        4, 3, 2,           // back
        1, 4, 2            // right
    };
    return output;
}
}  // namespace verna