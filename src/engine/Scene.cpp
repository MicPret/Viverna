#include <viverna/core/Scene.hpp>
#include <viverna/core/Assets.hpp>
#include <viverna/serialization/SceneSerializer.hpp>

namespace verna {

Scene& Scene::GetActive() {
    static Scene default_scene;
    return default_scene;
}

bool Scene::LoadFile(const std::filesystem::path& scene_file) {
    std::vector<Entity> placeholder;
    return LoadFile(scene_file, placeholder);
}

bool Scene::LoadFile(const std::filesystem::path& scene_file,
                     std::vector<Entity>& out_entities) {
    ReleaseResources();
    world.ClearData();
    const std::filesystem::path folder = "scenes";
    auto path = folder / scene_file;
    VERNA_LOGI("Loading " + path.string());
    auto raw = LoadRawAsset(path);
    auto yaml_string = std::string(raw.data(), raw.size());
    YAML::Node node = YAML::Load(yaml_string);
    return DeserializeScene(node, *this, out_entities);
}

void Scene::ReleaseResources() {
    texture_manager.FreeLoadedTextures();
    shader_manager.FreeLoadedShaders();
}

}  // namespace verna