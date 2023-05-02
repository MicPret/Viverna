#ifndef VERNA_GAME_MESH_RENDERER_HPP
#define VERNA_GAME_MESH_RENDERER_HPP

#include <viverna/core/Time.hpp>
#include <viverna/ecs/Entity.hpp>
#include <viverna/ecs/World.hpp>
#include <vector>

namespace editor {
void Render(verna::World& world, std::vector<verna::Entity>& entities);
}

#endif
