#ifndef CPPURSES_PAINTER_BRUSH_HPP
#define CPPURSES_PAINTER_BRUSH_HPP
#include <bitset>
#include <utility>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/color.hpp>

namespace cppurses {

/// Holds the look of any paintable object with Attributes and Colors.
class Brush {
   public:
    /// Construct a Brush with given Attributes and Colors.
    template <typename... Attributes>
    explicit Brush(Attributes... attrs) {
        this->add_attributes(std::forward<Attributes>(attrs)...);
    }

    // Base Case
    void add_attributes() {}

    /// Add a variable number of Attributes or Colors to the brush.
    /** Use the (back/fore)ground_color(Color c) functions to add colors to the
     *  list. */
    template <typename T, typename... Others>
    void add_attributes(T attr, Others... others) {
        this->set_attr(attr);
        this->add_attributes(others...);
    }

    /// Set the background color of this brush.
    void set_background(Color color) { background_color_ = color; }

    /// Set the foreground color of this brush.
    void set_foreground(Color color) { foreground_color_ = color; }

    /// Remove a specific Attribute, if it is set, otherwise no-op.
    void remove_attribute(Attribute attr) {
        attributes_.set(static_cast<std::int8_t>(attr), false);
    }

    /// Sets the background to not have a color, the default state.
    void remove_background() { background_color_ = opt::none; }

    /// Sets the foreground to not have a color, the default state.
    void remove_foreground() { foreground_color_ = opt::none; }

    /// Removes all of the set Attributes from the brush, not including colors.
    void clear_attributes() { attributes_.reset(); }

    /// Provides a check if the brush has the provided Attribute \p attr.
    bool has_attribute(Attribute attr) const {
        return attributes_[static_cast<std::size_t>(attr)];
    }

    /// Returns the current background as an opt::Optional object.
    opt::Optional<Color> background_color() const { return background_color_; }

    /// Returns the current foreground as an opt::Optional object.
    opt::Optional<Color> foreground_color() const { return foreground_color_; }

    friend bool operator==(const Brush& lhs, const Brush& rhs);

   private:
    /// Used by add_attributes() to set a deail::BackgroundColor.
    void set_attr(detail::BackgroundColor bc) {
        this->set_background(static_cast<Color>(bc));
    }

    /// Used by add_attributes() to set a deail::ForegroundColor.
    void set_attr(detail::ForegroundColor fc) {
        this->set_foreground(static_cast<Color>(fc));
    }

    /// Used by add_attributes() to set an Attribute.
    void set_attr(Attribute attr) {
        attributes_.set(static_cast<std::int8_t>(attr));
    }

    // Data Members
    std::bitset<8> attributes_;
    opt::Optional<Color> background_color_;
    opt::Optional<Color> foreground_color_;
};

/// Compares if the held attributes and (back/fore)ground colors are equal.
bool operator==(const Brush& lhs, const Brush& rhs);

/// Adds Attributes and Colors from \p from to \p to.
/** Does not overwrite existing colors in \p to. */
void imprint(const Brush& from, Brush& to);

}  // namespace cppurses
#endif  // CPPURSES_PAINTER_BRUSH_HPP
