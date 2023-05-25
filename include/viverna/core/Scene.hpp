#ifndef VERNA_SCENE_HPP
#define VERNA_SCENE_HPP

#include <viverna/ecs/World.hpp>
#include <viverna/graphics/Camera.hpp>
#include <viverna/graphics/TextureManager.hpp>
#include <viverna/graphics/ShaderManager.hpp>
#include <viverna/graphics/DirectionLight.hpp>

#include <filesystem>
#include <vector>

namespace verna {
struct Scene {
   public:
    Camera camera;
    DirectionLight direction_light;
    TextureManager texture_manager;
    ShaderManager shader_manager;
    World world;

    bool LoadFile(const std::filesystem::path& scene_file);
    bool LoadFile(const std::filesystem::path& scene_file,
                  std::vector<Entity>& out_entities);
    void ReleaseResources();
    static Scene& GetActive();
};
}  // namespace verna

#endif