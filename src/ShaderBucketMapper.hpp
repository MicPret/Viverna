#ifndef VERNA_SHADER_BUCKET_MAPPER_HPP
#define VERNA_SHADER_BUCKET_MAPPER_HPP

#include <vector>
#include <viverna/core/Debug.hpp>
#include <viverna/graphics/Shader.hpp>

namespace verna {
using BatchId = uint16_t;
using Bucket = std::vector<BatchId>;

// Every shader is paired with a bucket, which is a collection of indices;
// each of these indices is an offset for the batch buffer
class ShaderBucketMapper {
   public:
    // Check result against NotFound()
    std::vector<Bucket>::iterator FindBucket(ShaderId shader);
    // Check result against NotFound()
    std::vector<Bucket>::const_iterator FindBucket(ShaderId shader) const;
    bool NotFound(std::vector<Bucket>::iterator it) const;
    Bucket& NewBucket(ShaderId shader);
    size_t Size() const;
    bool Empty() const;
    void Get(size_t index, ShaderId& shader, Bucket& bucket) const;
    void Clear();

   private:
    std::vector<ShaderId> shaders;
    std::vector<Bucket> buckets;
};

inline std::vector<Bucket>::iterator ShaderBucketMapper::FindBucket(
    ShaderId shader) {
    for (size_t i = 0; i < shaders.size(); i++)
        if (shaders[i] == shader)
            return buckets.begin() + i;
    return buckets.end();
}
inline std::vector<Bucket>::const_iterator ShaderBucketMapper::FindBucket(
    ShaderId shader) const {
    for (size_t i = 0; i < shaders.size(); i++)
        if (shaders[i] == shader)
            return buckets.begin() + i;
    return buckets.end();
}
inline bool ShaderBucketMapper::NotFound(
    std::vector<Bucket>::iterator it) const {
    return it == buckets.end();
}
inline Bucket& ShaderBucketMapper::NewBucket(ShaderId shader) {
    shaders.push_back(shader);
    buckets.push_back(Bucket());
    return buckets.back();
}
inline size_t ShaderBucketMapper::Size() const {
    return shaders.size();
}
inline bool ShaderBucketMapper::Empty() const {
    return Size() == 0;
}
inline void ShaderBucketMapper::Get(size_t index,
                                    ShaderId& shader,
                                    Bucket& bucket) const {
    VERNA_ASSERT(index < Size());
    shader = shaders[index];
    bucket = buckets[index];
}
inline void ShaderBucketMapper::Clear() {
    shaders.clear();
    buckets.clear();
}
}  // namespace verna

#endif