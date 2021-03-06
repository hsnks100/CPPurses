#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_EXAMPLES_INFO_BOX_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_EXAMPLES_INFO_BOX_HPP
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/text_display.hpp>
#include <cppurses/widget/widgets/widget_stack.hpp>

namespace gol {

struct Examples_box : cppurses::Vertical_layout {
    Examples_box();

    cppurses::Push_button& example_1{
        this->make_child<cppurses::Push_button>("Example 1")};

    cppurses::Push_button& example_2{
        this->make_child<cppurses::Push_button>("Example 2")};

    cppurses::Push_button& example_3{
        this->make_child<cppurses::Push_button>("Example 3")};

    cppurses::Push_button& example_4{
        this->make_child<cppurses::Push_button>("Example 4")};

    cppurses::Push_button& example_5{
        this->make_child<cppurses::Push_button>("Example 5")};

    cppurses::Push_button& example_6{
        this->make_child<cppurses::Push_button>("Example 6")};

    cppurses::Push_button& example_7{
        this->make_child<cppurses::Push_button>("Example 7")};

    cppurses::Push_button& to_info_btn{
        this->make_child<cppurses::Push_button>("Rulesets")};
};

struct Info_box : cppurses::Vertical_layout {
    Info_box();

    cppurses::Text_display& info{
        this->make_child<cppurses::Text_display>("Rulesets go here")};
    cppurses::Push_button& to_examples_btn{
        this->make_child<cppurses::Push_button>("Patterns")};
};

/// Patterns_rulesets_box
struct Examples_info_box : public cppurses::Widget_stack {
    Examples_info_box();

    Examples_box& examples{this->make_page<Examples_box>()};
    Info_box& info{this->make_page<Info_box>()};
};

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_EXAMPLES_INFO_BOX_HPP
