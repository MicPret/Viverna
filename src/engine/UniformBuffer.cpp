#include "UniformBuffer.hpp"
#include <viverna/graphics/gpu/CameraData.hpp>
#include <viverna/graphics/gpu/MeshData.hpp>

#if defined(VERNA_DESKTOP)
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#elif defined(VERNA_ANDROID)
#include <GLES3/gl32.h>
#include <EGL/egl.h>
#else
#error Platform not supported!
#endif

#include <cstdint>
#include <vector>

namespace verna::ubo {

struct BlockInfo {
    uint32_t offset;
    size_t size;
    uint32_t binding_point;
};

static GLuint id;
static std::vector<BlockInfo> bindings;
static size_t base_offset_alignment;

void GenerateUBO() {
    GLint64 buffer_size;
    glGetInteger64v(GL_MAX_UNIFORM_BLOCK_SIZE, &buffer_size);
    glGenBuffers(1, &id);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
#if defined(VERNA_DESKTOP)
    glBufferStorage(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(buffer_size),
                    nullptr, GL_DYNAMIC_STORAGE_BIT);
#else
    glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(buffer_size),
                 nullptr, GL_DYNAMIC_DRAW);
#endif
    GLint temp;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &temp);
    base_offset_alignment = static_cast<size_t>(temp);
    bindings.reserve(2);
}

void TerminateUBO() {
    glDeleteBuffers(1, &id);
    bindings.clear();
}

void AddBlock(uint32_t binding_point, size_t size) {
    uint32_t offset = 0;
    if (!bindings.empty()) {
        const auto& last_block = bindings.back();
        offset = last_block.offset + last_block.size;
        auto alignment = sizeof(float) * 4;
        auto align_error = alignment % base_offset_alignment;
        if (align_error != 0)
            alignment += base_offset_alignment - align_error;
        align_error = offset % alignment;
        if (align_error != 0)
            offset += alignment - align_error;
    }
    glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, id, offset, size);
    BlockInfo new_block;
    new_block.offset = offset;
    new_block.size = size;
    new_block.binding_point = binding_point;
    VERNA_LOGI("Uniform Block | offset: " + std::to_string(offset)
               + ", size: " + std::to_string(size)
               + ", binding point: " + std::to_string(binding_point));
    bindings.push_back(new_block);
}

void SendData(uint32_t binding_point, const void* data) {
    for (const BlockInfo& block : bindings) {
        if (block.binding_point == binding_point) {
            glBufferSubData(GL_UNIFORM_BUFFER, block.offset, block.size, data);
            return;
        }
    }
}

}  // namespace verna::ubo