#ifndef VERNA_INPUT_HPP
#define VERNA_INPUT_HPP

#include "Key.hpp"
#include "VivernaState.hpp"

namespace verna {
/**
 * @brief Initializes the input system. Must be called after InitializeWindow()
 *
 * @param state
 */
void InitializeInput(VivernaState& state);
/**
 * @brief Terminates the input system. Must be called before TerminateWindow()
 *
 * @param state
 */
void TerminateInput(VivernaState& state);

class MouseListener {
   public:
    void Position(unsigned& pos_x, unsigned& pos_y) const;
    [[nodiscard]] bool Pressed(unsigned& pos_x, unsigned& pos_y) const;
};

class KeyListener {
   public:
    constexpr KeyListener(Key key_) : key(key_) {}
    bool Pressed() const;

   private:
    Key key;
};
}  // namespace verna

#endif