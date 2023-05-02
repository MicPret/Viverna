#ifndef VERNA_SPARSE_SET_HPP
#define VERNA_SPARSE_SET_HPP

#include <cstdint>
#include <vector>

namespace verna {
template <typename K>
class SparseSet {
   public:
    using index_t = uint32_t;
    void Add(K k);
    void Remove(K k);
    void Clear();
    auto Size() const { return dense.size(); }
    bool Contains(K k) const;
    bool GetIndex(K k, index_t& out_index) const;
    const auto& GetDense() const { return dense; }
    const auto& GetSparse() const { return sparse; }

   private:
    std::vector<K> dense;
    std::vector<index_t> sparse;
};

template <typename K>
void SparseSet<K>::Add(K k) {
    auto index = static_cast<index_t>(Size());
    dense.push_back(k);
    if (k >= sparse.size())
        sparse.resize(k + 1);
    sparse[k] = index;
}

template <typename K>
bool SparseSet<K>::Contains(K k) const {
    if (sparse.size() < k)
        return false;
    auto i = sparse[k];
    return i < Size() && dense[i] == k;
}

template <typename K>
bool SparseSet<K>::GetIndex(K k, index_t& out_index) const {
    if (sparse.size() < k)
        return false;
    out_index = sparse[k];
    return out_index < Size() && dense[out_index] == k;
}

template <typename K>
void SparseSet<K>::Remove(K k) {
    index_t i;
    if (GetIndex(k, i))
        sparse[k] = static_cast<index_t>(-1);
}

template <typename K>
void SparseSet<K>::Clear() {
    dense.clear();
    dense.shrink_to_fit();
    sparse.clear();
    sparse.shrink_to_fit();
}

}  // namespace verna

#endif
