#ifndef VERNA_GPU_BUFFER_HPP
#define VERNA_GPU_BUFFER_HPP

#include <cstdint>

namespace verna {
class GpuBuffer {
   public:
    using id_type = uint32_t;
    using buf_type = int32_t;
    constexpr GpuBuffer() : id(0), type(0), size(0), capacity(0) {}
    GpuBuffer(const GpuBuffer&) = delete;
    GpuBuffer(GpuBuffer&& other);
    GpuBuffer& operator=(const GpuBuffer&) = delete;
    GpuBuffer& operator=(GpuBuffer&& other);
    ~GpuBuffer();
    bool Initialize(buf_type buffer_type, size_t start_capacity);
    void Terminate();
    constexpr bool IsInitialized() const { return id != 0; }
    constexpr size_t Size() const { return size; }
    constexpr size_t Capacity() const { return capacity; }
    void PushBack(void* data, size_t bytes);
    void SetContent(void* data, size_t bytes);
    // No need to call it before other GpuBuffer methods
    void Bind();
    constexpr bool Bound() const { return id == bound_id; }

   private:
    id_type id;
    buf_type type;
    size_t size;
    size_t capacity;
    static id_type bound_id;
};
}  // namespace verna

#endif
