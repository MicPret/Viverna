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

bool DeserializeEntities(const YAML::Node& node,
                         World& out_world,
                         std::vector<Entity>& out_entities) {
    if (!node.IsMap())
        return false;
    out_world.Clear();
    out_entities.clear();
    for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
        EntityName name;
        name.str = it->first.as<std::string>(std::string());
        YAML::Node map = it->second;
        if (!map.IsMap())
            return false;

        YAML::Node material_node = map["material"];
        YAML::Node mesh_node = map["mesh"];
        YAML::Node shader_node = map["shader"];
        YAML::Node transform_node = map["transform"];
        bool failure =
            !material_node || !mesh_node || !shader_node || !transform_node;
        if (failure)
            return false;
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
                std::string obj_name = mesh_name.substr(0, index);
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

        Entity e = out_world.NewEntity<Material, Mesh, ShaderId, Transform>();
        out_world.SetComponents(e, material, mesh, shader, transform);
        out_entities.push_back(e);
    }

    return true;
}
}  // namespace verna
