#ifndef CPPURSES_WIDGET_FOCUS_POLICY_HPP
#define CPPURSES_WIDGET_FOCUS_POLICY_HPP

namespace cppurses {

/// Defines different ways a Widget can receiver the focus of the system.
/** None: Widget cannot have focus. */
/** Tab: Tab key presses will cycle the focus to Widgets with this policy. */
/** Click: A mouse click on this Widget will give it focus. */
/** Strong: Both Tab and Click policies apply. */
enum class Focus_policy { None, Tab, Click, Strong };

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_FOCUS_POLICY_HPP
