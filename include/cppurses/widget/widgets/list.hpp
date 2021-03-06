#ifndef CPPURSES_WIDGET_WIDGETS_LIST_HPP
#define CPPURSES_WIDGET_WIDGETS_LIST_HPP
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <signals/signals.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

/// Incomplete implementation, do not use.
template <typename T>
class List : public Widget {
   public:
    List();
    void add_property(Glyph_string property_name,
                      std::function<Glyph_string(const T&)> get_value);
    void add_item(const T& item);

    void rotate_properties();

    // Signals
    sig::Signal<void(T&)> selected;

   protected:
    bool paint_event() override;
    bool key_press_event(const Keyboard_data& keyboard) override;
    bool mouse_press_event(const Mouse_data& mouse) override;

   private:
    struct Property {
        Glyph_string name;
        std::function<Glyph_string(const T&)> get_value;
    };
    std::vector<T> items_;
    std::vector<Property> properties_;
    std::size_t selected_index_{1};

    void select_up(std::size_t n);
    void select_down(std::size_t n);
};

template <typename T>
List<T>::List() {
    this->focus_policy = Focus_policy::Strong;
}

template <typename T>
void List<T>::add_property(Glyph_string property_name,
                           std::function<Glyph_string(const T&)> get_value) {
    properties_.emplace_back(Property{property_name, get_value});
    this->update();
}

template <typename T>
void List<T>::add_item(const T& item) {
    items_.emplace_back(item);
    this->update();
}

template <typename T>
void List<T>::rotate_properties() {
    if (properties_.size() > 1) {
        std::rotate(std::begin(properties_), std::begin(properties_) + 1,
                    std::end(properties_));
    }
    this->update();
}

template <typename T>
bool List<T>::paint_event() {
    Painter p{*this};
    // p.move_cursor_on_put = true;
    for (const auto& prop : properties_) {  // below 0,0 might be wrong.
        p.put(Glyph_string{prop.name}.append(" | "), 0, 0);
    }
    std::size_t count{1};
    for (const auto& item : items_) {
        Glyph_string display;
        for (const auto& prop : properties_) {
            display.append(prop.get_value(item));
            display.append(" | ");
        }
        if (count == selected_index_) {
            display.add_attributes(Attribute::Bold);
        }
        p.put(display, 0, count);
        ++count;
    }
    this->cursor.set_position(Point{0, 0});
    return Widget::paint_event();
    move_cursor(*this, 0, 0);
}

template <typename T>
void List<T>::select_up(std::size_t n) {
    if (selected_index_ > n) {
        selected_index_ -= n;
    }
}

template <typename T>
void List<T>::select_down(std::size_t n) {
    if (items_.empty()) {
        return;
    }
    if (selected_index_ != items_.size()) {
        selected_index_ += n;
    }
    if (selected_index_ > items_.size()) {
        selected_index_ = items_.size();
    }
}

template <typename T>
bool List<T>::key_press_event(const Keyboard_data& keyboard) {
    if (keyboard.key == Key::Arrow_down || keyboard.key == Key::Arrow_left) {
        this->select_down(1);
    } else if (keyboard.key == Key::Arrow_up ||
               keyboard.key == Key::Arrow_right) {
        this->select_up(1);
    } else if (keyboard.key == Key::Enter) {
        if (!items_.empty()) {
            selected(items_.at(selected_index_ - 1));
        }
    }
    this->update();
    return Widget::key_press_event(keyboard);
}

template <typename T>
bool List<T>::mouse_press_event(const Mouse_data& mouse) {
    return Widget::mouse_press_event(mouse);
}

namespace slot {

template <typename T>
sig::Slot<void(const T&)> add_item(List<T>& list) {
    sig::Slot<void(const T&)> slot{
        [&list](const T& item) { list.add_item(item); }};
    slot.track(list.destroyed);
    return slot;
}

template <typename T>
sig::Slot<void()> add_item(List<T>& list, const T& item) {
    sig::Slot<void()> slot{[&list, item] { list.add_item(item); }};
    slot.track(list.destroyed);
    return slot;
}

template <typename T>
sig::Slot<void()> rotate_properties(List<T>& list) {
    sig::Slot<void()> slot{[&list] { list.rotate_properties(); }};
    slot.track(list.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LIST_HPP
