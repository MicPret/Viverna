#include <viverna/maths/MathUtils.hpp>
#include <viverna/core/Time.hpp>

#include <random>
#include <cmath>

namespace verna::maths {

using rng_engine = std::default_random_engine;

static rng_engine::result_type RNGSeed() {
    std::random_device rd;
    return rd.entropy() > 0.0 ? rd()
                              : static_cast<rng_engine::result_type>(
                                  Clock::now().time_since_epoch().count());
}

static rng_engine rng(RNGSeed());

float NextFloat(float x) {
    return std::nextafter(x, std::numeric_limits<float>::max());
}

float RandomFloatInclusive() {
    static std::uniform_real_distribution<float> inclusive_distribution(
        0.0f, NextFloat(1.0f));
    return inclusive_distribution(rng);
}

float RandomFloatExclusive() {
    static std::uniform_real_distribution<float> exclusive_distribution;
    return exclusive_distribution(rng);
}
}  // namespace verna::maths