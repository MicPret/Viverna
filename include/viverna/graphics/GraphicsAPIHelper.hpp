#ifndef VERNA_GRAPHICS_API_HELPER_HPP
#define VERNA_GRAPHICS_API_HELPER_HPP

#if defined(VERNA_ANDROID)
#include <GLES3/gl32.h>
#include <EGL/egl.h>
#elif defined(VERNA_DESKTOP)
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#endif

namespace verna {
bool InitGraphicsAPI();
void TermGraphicsAPI();
}  // namespace verna

#endif