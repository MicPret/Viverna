#ifndef VERNA_UNIFORM_BUFFER_HPP
#define VERNA_UNIFORM_BUFFER_HPP

#include <cstddef>
#include <cstdint>

namespace verna::ubo {
void GenerateUBO();
void TerminateUBO();
void AddBlock(uint32_t binding_point, size_t size);
void SendData(uint32_t binding_point, const void* data);
}  // namespace verna::ubo

#endif