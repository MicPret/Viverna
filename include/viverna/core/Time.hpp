#ifndef VERNA_TIME_HPP
#define VERNA_TIME_HPP

/**
 * @file Time.hpp
 * @brief As of now, this is just a user-friendly aliasing of chrono
 * Usage:
 * verna::TimePoint earlier = verna::Clock::now();
 * verna::TimePoint later = verna::Clock::now() + verna::Millis(1003);
 * verna::DeltaTime<float, verna::Seconds> delta_time = later - earlier;
 * float seconds = delta_time.count();
 *
 */

#include <chrono>

namespace verna {

using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Seconds = std::chrono::seconds;
using Milliseconds = std::chrono::milliseconds;
using Nanoseconds = std::chrono::nanoseconds;
template <typename Repr, typename Measure>
using DeltaTime = std::chrono::duration<Repr, typename Measure::period>;

}  // namespace verna

#endif