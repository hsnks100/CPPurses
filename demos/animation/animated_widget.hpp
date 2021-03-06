#ifndef CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
#define CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
#include <cstdint>

#include <cppurses/cppurses.hpp>

namespace demos {
namespace animation {

class Animated_bit : public cppurses::Widget {
   public:
    Animated_bit(int rate, bool ani = false);

    bool timer_event() override;
    bool paint_event() override;

    bool mouse_press_event(const cppurses::Mouse_data& mouse) override;

   private:
    cppurses::Glyph glyph_{'X', cppurses::foreground(cppurses::Color::Yellow)};
    cppurses::Point coords_{0, 0};
    int x_direction{1};
    int y_direction{1};
};

class Animated_widget : public cppurses::Horizontal_layout {
   public:
    Animated_widget() {
        cppurses::enable_border(bit_2_);
        cppurses::enable_border(bit_4_);
    }

   private:
    Animated_bit& bit_1_{this->make_child<Animated_bit>(5, true)};
    Animated_bit& bit_2_{this->make_child<Animated_bit>(10, true)};
    Animated_bit& bit_3_{this->make_child<Animated_bit>(20, true)};
    Animated_bit& bit_4_{this->make_child<Animated_bit>(30, true)};
    Animated_bit& bit_5_{this->make_child<Animated_bit>(60, true)};
};

}  // namespace animation
}  // namespace demos
#endif  // CPPURSES_DEMOS_ANIMATION_ANIMATED_WIDGET_HPP
