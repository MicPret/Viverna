#include <viverna/graphics/GraphicsAPIHelper.hpp>
#include <viverna/graphics/gpu/ImplementationInfo.hpp>

namespace verna::gpu {
int MaxTextureUnits() {
    static GLint max_frag_tus = [] {
        GLint x;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &x);
        return x;
    }();
    return max_frag_tus;
}
}  // namespace verna::gpu