#include <viverna/core/Scene.hpp>
#include <viverna/core/Assets.hpp>
#include <viverna/core/Debug.hpp>
#include <viverna/serialization/SceneSerializer.hpp>

#include <fstream>

namespace verna {

static bool ValidFileName(std::string_view name);

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
    auto name = scene_file.string();
    if (!ValidFileName(name))
        name += ".viv";
    auto path = folder / name;
    VERNA_LOGI("Loading " + path.string());
    auto raw = LoadRawAsset(path);
    auto yaml_string = std::string(raw.data(), raw.size());
    YAML::Node node = YAML::Load(yaml_string);
    return DeserializeScene(node, *this, out_entities);
}

void Scene::SaveFile(const std::filesystem::path& new_file) {
    auto name = new_file.string();
    if (!ValidFileName(name))
        name += ".viv";
    YAML::Emitter emitter;
    SerializeScene(emitter, *this);
    std::ofstream file(name);
    file << "# viv 0.4\n" << emitter.c_str();
}

void Scene::ReleaseResources() {
    texture_manager.FreeLoadedTextures();
    shader_manager.FreeLoadedShaders();
}

// static functions

bool ValidFileName(std::string_view name) {
    if (name.length() <= 4)
        return false;
    auto ext = name.substr(name.length() - 4);
    return (ext == ".viv") || (ext == ".VIV");
}

}  // namespace verna