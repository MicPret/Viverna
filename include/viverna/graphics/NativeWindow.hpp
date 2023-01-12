#ifndef VERNA_NATIVE_WINDOW_HPP
#define VERNA_NATIVE_WINDOW_HPP

namespace verna {
void SetNativeWindowPointer(void* ptr);
void* GetNativeWindowPointer();
void SwapNativeWindowBuffers();
int WindowWidth();
int WindowHeight();
}  // namespace verna

#endif