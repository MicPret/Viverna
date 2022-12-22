#ifndef VERNA_VERTEX_HPP
#define VERNA_VERTEX_HPP

#include <viverna/maths/Vec2f.hpp>
#include <viverna/maths/Vec3f.hpp>

namespace verna {

struct Vertex {
    Vec3f position;
    Vec2f texture_coords;
};

}  // namespace verna

#endif