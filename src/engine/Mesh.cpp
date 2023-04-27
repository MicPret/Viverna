#include <viverna/graphics/Mesh.hpp>
#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/maths/Quaternion.hpp>

#include <sstream>
#include <utility>

namespace verna {

static Vec3f CalculateNormal(const Vec3f& a, const Vec3f& b, const Vec3f& c);
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
        Vec3f normal = CalculateNormal(
            vertices[a].position, vertices[b].position, vertices[c].position);
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

// OBJ
struct ObjVert {
    unsigned pos_id;
    unsigned tex_coords_id;
    unsigned norm_id;
};
struct ObjTri {
    std::array<ObjVert, 3> verts;
};
static Mesh MakeMesh(const std::vector<Vec3f>& positions,
                     const std::vector<Vec2f>& tex_coords,
                     const std::vector<Vec3f>& normals,
                     const std::vector<ObjTri>& tris);

std::vector<Mesh> LoadMeshesOBJ(std::filesystem::path& mesh_path) {
    std::filesystem::path path = "meshes" / mesh_path;

    auto raw = LoadRawAsset(path);
    if (raw.empty()) {
        VERNA_LOGE("Failed to load mesh at " + path.string());
        return {};
    }
    std::vector<Mesh> result;
    std::stringstream objstream(std::string(raw.data(), raw.size()));

    std::string line;
    std::vector<Vec3f> positions;
    std::vector<Vec2f> tex_coords;
    std::vector<Vec3f> normals;
    std::vector<ObjTri> tris;
    positions.reserve(8);
    tex_coords.reserve(8);
    normals.reserve(8);
    tris.reserve(8);
    while (std::getline(objstream, line)) {
        std::stringstream linestream(line);
        std::string token;
        linestream >> token;
        if (token == "v") {
            Vec3f v;
            linestream >> v.x >> v.y >> v.z;
            positions.push_back(v);
        } else if (token == "vt") {
            Vec2f vt;
            linestream >> vt.x >> vt.y;
            tex_coords.push_back(vt);
        } else if (token == "vn") {
            Vec3f vn;
            linestream >> vn.x >> vn.y >> vn.z;
            normals.push_back(vn);
        } else if (token == "f") {
            std::vector<std::string> tokens;
            tokens.reserve(4);
            while (linestream >> token)
                tokens.push_back(token);
            std::vector<ObjVert> verts;
            verts.resize(tokens.size());
            for (size_t i = 0; i < tokens.size(); i++) {
                std::stringstream tk(std::move(tokens[i]));
                ObjVert v;
                if (std::getline(tk, token, '/'))
                    v.pos_id = std::stoul(token) - 1;
                if (std::getline(tk, token, '/') && !token.empty())
                    v.tex_coords_id = std::stoul(token) - 1;
                if (std::getline(tk, token, '/') && !token.empty())
                    v.norm_id = std::stoul(token) - 1;
                verts[i] = v;
            }
            switch (verts.size()) {
                case 4:
                    tris.push_back({verts[2], verts[3], verts[0]});
                    [[fallthrough]];
                case 3:
                    tris.push_back({verts[0], verts[1], verts[2]});
                    break;
                default:
                    VERNA_LOGE("Unsupported number of faces in " + path.string()
                               + ":\n" + line);
                    return {};
            }
        } else if (token == "o" || token == "g") {
            if (positions.empty())
                continue;
            result.push_back(MakeMesh(positions, tex_coords, normals, tris));
            positions.clear();
            tex_coords.clear();
            normals.clear();
            tris.clear();
        } else {
            VERNA_LOGE("Unrecognized token while parsing " + path.string()
                       + ": " + token);
            return {};
        }
    }
    if (!positions.empty()) {
        result.push_back(MakeMesh(positions, tex_coords, normals, tris));
        positions.clear();
        tex_coords.clear();
        normals.clear();
        tris.clear();
    }

    return result;
}

Mesh MakeMesh(const std::vector<Vec3f>& positions,
              const std::vector<Vec2f>& tex_coords,
              const std::vector<Vec3f>& normals,
              const std::vector<ObjTri>& tris) {
    Mesh m;
    m.vertices.reserve(positions.size());
    m.indices.reserve(tris.size() * 3);
    for (size_t i = 0; i < tris.size(); i++) {
        const ObjTri& tri = tris[i];
        std::array<Vertex, 3> face;
        for (size_t j = 0; j < 3; j++) {
            Vertex v;
            v.position = positions[tri.verts[j].pos_id];
            v.texture_coords = tex_coords[tri.verts[j].tex_coords_id];
            v.normal = normals[tri.verts[j].norm_id];
            face[j] = v;
        }
        m.vertices.insert(m.vertices.end(), face.begin(), face.end());
        auto k = static_cast<Mesh::index_t>(i * 3);
        m.indices.insert(m.indices.end(), {k, k + 1, k + 2});
    }
    return m;
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
    constexpr size_t N_VERTICES = 24;
    output.vertices.resize(N_VERTICES);
    // front
    for (size_t i = 0; i < 4; i++)
        output.vertices[i].normal = -Vec3f::UnitZ();
    output.vertices[0].position = Vec3f(-0.5f, 0.5f, -0.5f);   // top left
    output.vertices[1].position = Vec3f(-0.5f, -0.5f, -0.5f);  // btm left
    output.vertices[2].position = Vec3f(0.5f, -0.5f, -0.5f);   // btm right
    output.vertices[3].position = Vec3f(0.5f, 0.5f, -0.5f);    // top right
    // left
    for (size_t i = 4; i < 8; i++)
        output.vertices[i].normal = -Vec3f::UnitX();
    output.vertices[4].position = Vec3f(-0.5f, 0.5f, 0.5f);    // top left
    output.vertices[5].position = Vec3f(-0.5f, -0.5f, 0.5f);   // btm left
    output.vertices[6].position = Vec3f(-0.5f, -0.5f, -0.5f);  // btm right
    output.vertices[7].position = Vec3f(-0.5f, 0.5f, -0.5f);   // top right
    // back
    for (size_t i = 8; i < 12; i++)
        output.vertices[i].normal = Vec3f::UnitZ();
    output.vertices[8].position = Vec3f(0.5f, 0.5f, 0.5f);     // top left
    output.vertices[9].position = Vec3f(0.5f, -0.5f, 0.5f);    // btm left
    output.vertices[10].position = Vec3f(-0.5f, -0.5f, 0.5f);  // btm right
    output.vertices[11].position = Vec3f(-0.5f, 0.5f, 0.5f);   // top right
    // right
    for (size_t i = 12; i < 16; i++)
        output.vertices[i].normal = Vec3f::UnitX();
    output.vertices[12].position = Vec3f(0.5f, 0.5f, -0.5f);   // top left
    output.vertices[13].position = Vec3f(0.5f, -0.5f, -0.5f);  // btm left
    output.vertices[14].position = Vec3f(0.5f, -0.5f, 0.5f);   // btm right
    output.vertices[15].position = Vec3f(0.5f, 0.5f, 0.5f);    // top right
    // top
    for (size_t i = 16; i < 20; i++)
        output.vertices[i].normal = Vec3f::UnitY();
    output.vertices[16].position = Vec3f(-0.5f, 0.5f, 0.5f);   // top left
    output.vertices[17].position = Vec3f(-0.5f, 0.5f, -0.5f);  // btm left
    output.vertices[18].position = Vec3f(0.5f, 0.5f, -0.5f);   // btm right
    output.vertices[19].position = Vec3f(0.5f, 0.5f, 0.5f);    // top right
    // bottom
    for (size_t i = 20; i < 24; i++)
        output.vertices[i].normal = -Vec3f::UnitY();
    output.vertices[20].position = Vec3f(-0.5f, -0.5f, -0.5f);  // top left
    output.vertices[21].position = Vec3f(-0.5f, -0.5f, 0.5f);   // btm left
    output.vertices[22].position = Vec3f(0.5f, -0.5f, 0.5f);    // btm right
    output.vertices[23].position = Vec3f(0.5f, -0.5f, -0.5f);   // top right
    // texture_coords
    for (size_t i = 0; i < N_VERTICES; i += 4) {
        output.vertices[i].texture_coords = Vec2f(0.0f, 1.0f);
        output.vertices[i + 1].texture_coords = Vec2f(0.0f, 0.0f);
        output.vertices[i + 2].texture_coords = Vec2f(1.0f, 0.0f);
        output.vertices[i + 3].texture_coords = Vec2f(1.0f, 1.0f);
    }
    // indices
    constexpr size_t N_INDICES = 36;
    output.indices.resize(N_INDICES);
    std::array<Mesh::index_t, 6> triangles = {0, 1, 2, 2, 3, 0};
    for (size_t i = 0; i < N_INDICES; i++) {
        size_t j = i % 6;
        size_t k = i / 6;
        output.indices[i] = triangles[j] + (k * 4);
    }

    output.RecalculateBounds();
    return output;
}

static Mesh LoadPrimitivePyramid() {
    Mesh output;
    constexpr size_t N_VERTICES = 16;
    constexpr size_t N_INDICES = 18;
    output.vertices.resize(N_VERTICES);
    output.indices.reserve(N_INDICES);
    // base
    output.vertices[0].position = Vec3f(-0.5f);
    output.vertices[0].texture_coords = Vec2f(0.0f, 1.0f);
    output.vertices[1].position = Vec3f(-0.5f, -0.5f, 0.5f);
    output.vertices[1].texture_coords = Vec2f(0.0f, 0.0f);
    output.vertices[2].position = Vec3f(0.5f, -0.5f, 0.5f);
    output.vertices[2].texture_coords = Vec2f(1.0f, 0.0f);
    output.vertices[3].position = Vec3f(0.5f, -0.5f, -0.5f);
    output.vertices[3].texture_coords = Vec2f(1.0f, 1.0f);
    for (size_t i = 0; i < 4; i++)
        output.vertices[i].normal = -Vec3f::UnitY();

    Vec3f top(0.0f, 0.5f, 0.0f);
    // front
    output.vertices[4].position = Vec3f(-0.5f);
    output.vertices[5].position = Vec3f(0.5f, -0.5f, -0.5f);
    output.vertices[6].position = top;
    // right
    output.vertices[7].position = Vec3f(0.5f, -0.5f, -0.5f);
    output.vertices[8].position = Vec3f(0.5f, -0.5f, 0.5f);
    output.vertices[9].position = top;
    // back
    output.vertices[10].position = Vec3f(0.5f, -0.5f, 0.5f);
    output.vertices[11].position = Vec3f(-0.5f, -0.5f, 0.5f);
    output.vertices[12].position = top;
    // left
    output.vertices[13].position = Vec3f(-0.5f, -0.5f, 0.5f);
    output.vertices[14].position = Vec3f(-0.5f);
    output.vertices[15].position = top;
    // edge tex_coords and normals
    for (size_t i = 4; i < N_VERTICES; i += 3) {
        Vertex& a = output.vertices[i];
        Vertex& b = output.vertices[i + 1];
        Vertex& c = output.vertices[i + 2];
        a.texture_coords = Vec2f(0.0f, 0.0f);
        b.texture_coords = Vec2f(1.0f, 0.0f);
        c.texture_coords = Vec2f(0.5f, 1.0f);
        Vec3f normal = CalculateNormal(a.position, b.position, c.position);
        a.normal = normal;
        b.normal = normal;
        c.normal = normal;
    }

    // indices
    output.indices.insert(output.indices.end(), {0, 1, 2, 2, 3, 0});
    for (Mesh::index_t i = 4; i < N_INDICES; i += 3)
        output.indices.insert(output.indices.end(), {i, i + 1, i + 2});

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

Vec3f CalculateNormal(const Vec3f& a, const Vec3f& b, const Vec3f& c) {
    return (a - b).Cross(c - b);
}

}  // namespace verna
