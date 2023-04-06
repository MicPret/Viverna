#include <viverna/graphics/Mesh.hpp>
#include <viverna/core/Debug.hpp>

namespace verna {

void Mesh::RecalculateNormals() {
    if (vertices.size() < 3 || indices.size() < 3) {
        VERNA_LOGE("RecalculateNormals failed: there is no triangle!");
        return;
    }

    // TODO test!!!
    for (auto& v : vertices)
        v.normal = Vec3f();
    size_t max_i = indices.size() - 2;
    for (size_t i = 0; i < max_i; i += 3) {
        auto a = indices[i];
        auto b = indices[i + 1];
        auto c = indices[i + 2];
        Vec3f v1 = vertices[a].position - vertices[b].position;
        Vec3f v2 = vertices[c].position - vertices[b].position;
        Vec3f normal = v1.Cross(v2);
        vertices[a].normal += normal;
        vertices[b].normal += normal;
        vertices[c].normal += normal;
    }
    for (Vertex& v : vertices)
        v.normal = v.normal.Normalized();
}

void Mesh::RecalculateBounds() {
    bounds.Recalculate(*this);
}

// Primitives

static Mesh LoadPrimitiveCube();
static Mesh LoadPrimitivePyramid();
static Mesh LoadPrimitiveSphere();

Mesh LoadPrimitiveMesh(PrimitiveMeshType type) {
    switch (type) {
        case PrimitiveMeshType::Cube:
            return LoadPrimitiveCube();
        case PrimitiveMeshType::Pyramid:
            return LoadPrimitivePyramid();
        case PrimitiveMeshType::Sphere:
            return LoadPrimitiveSphere();
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
    output.RecalculateNormals();
    output.RecalculateBounds();
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
    output.RecalculateNormals();
    output.RecalculateBounds();
    return output;
}

static Mesh LoadPrimitiveSphere() {
    constexpr unsigned SECTIONS = 8;
    Mesh sphere;
    sphere.vertices.resize(SECTIONS * SECTIONS + 2);
    Vertex vtx;
    vtx.position = Vec3f::UnitY();
    vtx.texture_coords = Vec2f(0.0f, 1.0f);
    sphere.vertices.front() = vtx;
    vtx.position = -Vec3f::UnitY();
    vtx.texture_coords = Vec2f(0.0f, 0.0f);
    sphere.vertices.back() = vtx;
    constexpr float n = maths::Pi() / static_cast<float>(SECTIONS);
    constexpr float n2 = 2.0f * n;
    for (unsigned i = 0; i < SECTIONS; i++) {
        float zenit = n * static_cast<float>(i);
        for (unsigned j = 0; j < SECTIONS; j++) {
            float azimut = n2 * static_cast<float>(j);
            vtx.position = Vec3f::FromPolarCoordinates(azimut, zenit);
            vtx.texture_coords =
                Vec2f(static_cast<float>(j) / static_cast<float>(SECTIONS),
                      static_cast<float>(i) / static_cast<float>(SECTIONS));
            unsigned index = i * SECTIONS + j + 1;
            sphere.vertices[index] = vtx;
        }
    }
    for (unsigned i = 0; i < SECTIONS; i++) {
        unsigned i0 = i + 1;
        unsigned i1 = (i + 1) % SECTIONS + 1;
        sphere.indices.insert(sphere.indices.end(), {0, i1, i0});
        i0 = i + SECTIONS * (SECTIONS - 2) + 1;
        i1 = (i + 1) % SECTIONS + SECTIONS * (SECTIONS - 2) + 1;
        sphere.indices.insert(
            sphere.indices.end(),
            {static_cast<Mesh::index_t>(sphere.vertices.size() - 1), i0, i1});
    }
    for (unsigned i = 0; i < SECTIONS - 2; i++) {
        unsigned i0 = i * SECTIONS + 1;
        unsigned i1 = (i + 1) * SECTIONS + 1;
        for (unsigned j = 0; j < SECTIONS; j++) {
            unsigned j0 = i0 + j;
            unsigned j1 = i0 + (j + 1) % SECTIONS;
            unsigned j2 = i1 + (j + 1) % SECTIONS;
            unsigned j3 = i1 + j;
            sphere.indices.insert(sphere.indices.end(),
                                  {j0, j1, j2, j2, j3, j0});
        }
    }
    sphere.RecalculateNormals();
    sphere.RecalculateBounds();
    return sphere;
}

}  // namespace verna
