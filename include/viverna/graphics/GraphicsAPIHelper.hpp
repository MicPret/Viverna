#ifndef VERNA_GRAPHICS_API_HELPER_HPP
#define VERNA_GRAPHICS_API_HELPER_HPP

#if defined(VERNA_ANDROID)
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#elif defined(VERNA_DESKTOP)
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#endif

namespace verna {
bool InitGraphicsAPI();
void TermGraphicsAPI();
}  // namespace verna

#endif