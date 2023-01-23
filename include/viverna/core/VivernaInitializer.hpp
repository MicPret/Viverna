#ifndef VERNA_VIVERNA_INITIALIZER_HPP
#define VERNA_VIVERNA_INITIALIZER_HPP

#include "VivernaState.hpp"

namespace verna {
void InitializeAll(VivernaState& state);
void TerminateAll(VivernaState& state);

struct VivernaInitializerRAII {
    VivernaState app_state;
    VivernaInitializerRAII() { InitializeAll(app_state); }
    ~VivernaInitializerRAII() { TerminateAll(app_state); }
};
}  // namespace verna

#endif