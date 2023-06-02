#include <game/NewCube.hpp>
#include <viverna/core/Scene.hpp>
#include <viverna/core/Transform.hpp>
#include <viverna/ecs/EntityName.hpp>
#include <viverna/graphics/Material.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/graphics/Shader.hpp>

namespace editor {

static verna::Material DefaultMaterial();
static verna::ShaderId DefaultShader();

verna::Entity NewCube(verna::World& world) {
    verna::EntityName name("New entity");
    verna::Material mat = DefaultMaterial();
    verna::Mesh cube = verna::LoadPrimitiveMesh(verna::PrimitiveMeshType::Cube);
    verna::ShaderId shader = DefaultShader();

    auto e = world.NewEntity<verna::EntityName, verna::Material, verna::Mesh,
                             verna::ShaderId, verna::Transform>();
    world.SetComponents(e, name, mat, cube, shader, verna::Transform());
    return e;
}

// static functions

verna::Material DefaultMaterial() {
    auto& scene = verna::Scene::GetActive();
    auto diffuse = scene.texture_manager.LoadTextureFromColor(
        verna::Color4f(0.7f, 0.7f, 0.7f, 1.0f), {});
    auto specular = scene.texture_manager.LoadTextureFromColor(
        verna::Color4f(0.4f, 0.4f, 0.4f, 1.0f), {});
    verna::Material mat;
    mat.textures[verna::Material::DIFFUSE_INDEX] = diffuse;
    mat.textures[verna::Material::SPECULAR_INDEX] = specular;
    mat.parameters[verna::Material::SHININESS_INDEX] = 20.06f;
    return mat;
}

verna::ShaderId DefaultShader() {
    auto& scene = verna::Scene::GetActive();
    return scene.shader_manager.LoadShader("blinn-phong");
}

}  // namespace editor
