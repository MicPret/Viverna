#include <viverna/serialization/WorldSerializer.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/ecs/EntityName.hpp>
#include <viverna/graphics/Mesh.hpp>
#include <viverna/serialization/MaterialSerializer.hpp>
#include <viverna/serialization/ShaderSerializer.hpp>
#include <viverna/serialization/TransformSerializer.hpp>

namespace verna {
YAML::Emitter& SerializeEntities(YAML::Emitter& emitter,
                                 const World& world,
                                 const std::vector<Entity>& entities) {
    EntityName name;
    Material material;
    Mesh mesh;
    ShaderId shader;
    Transform transform;
    emitter << YAML::BeginMap;
    for (size_t i = 0; i < entities.size(); i++) {
        world.GetComponents(entities[i], name, material, mesh, shader,
                            transform);
        emitter << YAML::Key << name.str;
        emitter << YAML::Value << YAML::BeginMap;
        emitter << YAML::Key << "material" << YAML::Value << material;
        emitter << YAML::Key << "mesh" << YAML::Value << GetMeshName(mesh.id);
        emitter << YAML::Key << "shader" << YAML::Value << shader;
        emitter << YAML::Key << "transform" << YAML::Value << transform;
        emitter << YAML::EndMap;
    }
    emitter << YAML::EndMap;
    return emitter;
}

static void FreeResources(World& world);

bool DeserializeEntities(const YAML::Node& node,
                         World& out_world,
                         std::vector<Entity>& out_entities) {
    if (!node.IsMap()) {
        VERNA_LOGE("Entities node is not a map!");
        return false;
    }
    FreeResources(out_world);
    out_world.ClearData();
    out_entities.clear();
    for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
        EntityName name;
        name.str = it->first.as<std::string>(std::string());
        YAML::Node map = it->second;
        if (!map.IsMap()) {
            VERNA_LOGE(name.str + " node is not a map!");
            return false;
        }

        YAML::Node material_node = map["material"];
        if (!material_node) {
            VERNA_LOGE("Material node not found!");
            return false;
        }
        YAML::Node mesh_node = map["mesh"];
        if (!mesh_node) {
            VERNA_LOGE("Mesh node not found!");
            return false;
        }
        YAML::Node shader_node = map["shader"];
        if (!shader_node) {
            VERNA_LOGE("Shader node not found!");
            return false;
        }
        YAML::Node transform_node = map["transform"];
        if (!transform_node) {
            VERNA_LOGE("Transform node not found!");
            return false;
        }
        Material material = material_node.as<Material>(Material());
        std::string mesh_name = mesh_node.as<std::string>(std::string());
        Mesh mesh;  // TODO optimize
        if (mesh_name == "CUBE")
            mesh = LoadPrimitiveMesh(PrimitiveMeshType::Cube);
        else if (mesh_name == "PYRAMID")
            mesh = LoadPrimitiveMesh(PrimitiveMeshType::Pyramid);
        else if (mesh_name == "SPHERE")
            mesh = LoadPrimitiveMesh(PrimitiveMeshType::Sphere);
        else if (mesh_name.length() > 6) {
            std::string ext = mesh_name.substr(mesh_name.length() - 4);
            auto index = mesh_name.rfind(".obj##");
            if (index == std::string::npos)
                index = mesh_name.rfind(".OBJ##");
            if (index != std::string::npos) {
                std::string obj_name = mesh_name.substr(0, index + 4);
                auto meshes = LoadMeshesOBJ(obj_name);
                for (const Mesh& m : meshes) {
                    if (GetMeshName(m.id) == mesh_name) {
                        mesh = m;
                        break;
                    }
                }
            }
        } else {
            VERNA_LOGE("Failed to deserialize the following mesh: "
                       + mesh_name);
        }
        ShaderId shader = shader_node.as<ShaderId>(ShaderId());
        Transform transform = transform_node.as<Transform>(Transform());

        Entity e =
            out_world
                .NewEntity<EntityName, Material, Mesh, ShaderId, Transform>();
        out_world.SetComponents(e, name, material, mesh, shader, transform);
        out_entities.push_back(e);
    }

    return true;
}

void FreeResources(World& world) {
    auto entities = world.GetEntitiesWithComponent<Material>();
    for (Entity e : entities) {
        auto material = world.GetComponent<Material>(e);
        for (TextureId t : material.textures)
            FreeTexture(t);
    }
    entities = world.GetEntitiesWithComponent<ShaderId>();
    for (Entity e : entities) {
        auto shader = world.GetComponent<ShaderId>(e);
        FreeShader(shader);
    }
}
}  // namespace verna
