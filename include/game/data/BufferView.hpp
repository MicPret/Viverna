#ifndef VERNA_GAME_BUFFER_VIEW_HPP
#define VERNA_GAME_BUFFER_VIEW_HPP

namespace snake {
template <typename T>
struct BufferView {
    const T* data;
    size_t size;
    BufferView() : data(nullptr), size(0) {}
    BufferView(const T* data_, size_t size_) : data(data_), size(size_) {}
    const T* begin() const { return data; }
    const T* end() const { return data + size; }
};
}  // namespace snake
#endif