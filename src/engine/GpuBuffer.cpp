#include <viverna/data/GpuBuffer.hpp>
#include <viverna/core/Debug.hpp>

#if defined(VERNA_DESKTOP)
#include <glad/gl.h>
#elif defined(VERNA_ANDROID)
#include <GLES3/gl32.h>
#else
#error Platform not supported!
#endif

#include <algorithm>
#include <new>
#include <utility>

#define VERNA_CHECK_GPUB_INIT()                                       \
    VERNA_LOGE_IF(                                                    \
        !IsInitialized(),                                             \
        std::string("Tried to use GpuBuffer before initialization: ") \
            + __func__)

namespace verna {

GpuBuffer::id_type GpuBuffer::bound_id = 0;

GpuBuffer::GpuBuffer(GpuBuffer&& other) :
    id(other.id), size(other.size), capacity(other.capacity) {
    other.id = 0;
    other.size = 0;
    other.capacity = 0;
}

GpuBuffer& GpuBuffer::operator=(GpuBuffer&& other) {
    Terminate();
    new (this) GpuBuffer(std::move(other));
}

GpuBuffer::~GpuBuffer() {
    VERNA_LOGE_IF(IsInitialized(),
                  "GpuBuffer not terminated: (" + std::to_string(id) + ')');
}

bool GpuBuffer::Initialize(buf_type buffer_type, size_t init_capacity) {
    if (IsInitialized())
        return true;
    if (init_capacity == 0)
        return false;
    glGenBuffers(1, &id);
    if (id == 0)
        return false;
    Bind();
    glBufferData(buffer_type, init_capacity, nullptr, GL_DYNAMIC_DRAW);
    auto err = glGetError();
    switch (err) {
        case GL_NO_ERROR:
            capacity = init_capacity;
            type = buffer_type;
            break;
        case GL_INVALID_ENUM:
            VERNA_LOGE("Failed to initialize GpuBuffer: wrong buffer_type!");
            Terminate();
            break;
        case GL_OUT_OF_MEMORY:
            VERNA_LOGE("Failed to initialize GpuBuffer: GL_OUT_OF_MEMORY!");
            Terminate();
            break;
        default:
            VERNA_LOGE("Failed to initialize GpuBuffer! Error code: "
                       + std::to_string(err));
            Terminate();
            break;
    }
    return IsInitialized();
}

void GpuBuffer::Terminate() {
    if (!IsInitialized())
        return;
    glDeleteBuffers(1, &id);
    new (this) GpuBuffer();
}

void GpuBuffer::PushBack(const void* data, size_t bytes) {
    VERNA_CHECK_GPUB_INIT();
    Bind();
    auto total_size = size + bytes;
    if (total_size < capacity)
        glBufferSubData(type, size, bytes, data);
    else {
        auto new_cap = std::max(capacity * 3 / 2, total_size);
        void* tmp = operator new(new_cap, std::nothrow);
        glGetBufferSubData(type, 0, size, tmp);
        std::copy_n(reinterpret_cast<const uint8_t*>(data), bytes,
                    reinterpret_cast<uint8_t*>(tmp) + size);
        glBufferData(type, new_cap, tmp, GL_DYNAMIC_DRAW);
        operator delete(tmp);
        capacity = new_cap;
    }
    size = total_size;
}

void GpuBuffer::SetContent(const void* data, size_t bytes) {
    VERNA_CHECK_GPUB_INIT();
    Bind();
    if (capacity < bytes) {
        glBufferData(type, bytes, data, GL_DYNAMIC_DRAW);
        capacity = bytes;
    } else {
        glBufferSubData(type, 0, bytes, data);
    }
    size = bytes;
}

void GpuBuffer::Bind() {
    if (Bound())
        return;
    glBindBuffer(type, id);
    bound_id = id;
}

bool GpuBuffer::Bound() const {
    return id == bound_id;
}

}  // namespace verna
