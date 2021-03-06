#ifndef CPPURSES_WIDGET_WIDGETS_TEXT_DISPLAY_HPP
#define CPPURSES_WIDGET_WIDGETS_TEXT_DISPLAY_HPP
#include <cstddef>
#include <utility>
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

enum class Alignment { Left, Center, Right };

class Text_display : public Widget {
   public:
    explicit Text_display(Glyph_string content = "")
        : contents_{std::move(content)} {}

    void update() override;

    // Text Modification
    void set_text(Glyph_string text);
    void insert(Glyph_string text, std::size_t index);
    void append(Glyph_string text);
    void erase(std::size_t index, std::size_t length = Glyph_string::npos);
    void pop_back();
    void clear();

    void set_alignment(Alignment type) {
        alignment_ = type;
        this->update();
    }

    Alignment alignment() const { return alignment_; }

    // Scrolling
    virtual void scroll_up(std::size_t n = 1);
    virtual void scroll_down(std::size_t n = 1);

    // Word Wrapping
    void enable_word_wrap(bool enable = true) {
        word_wrap_ = enable;
        this->update();
    }

    void disable_word_wrap(bool disable = true) {
        word_wrap_ = !disable;
        this->update();
    }

    void toggle_word_wrap() {
        word_wrap_ = !word_wrap_;
        this->update();
    }

    // Incoming Text Attributes
    void add_new_text_attribute(Attribute attr) {
        new_text_brush_.add_attributes(attr);
    }

    void remove_new_text_attribute(Attribute attr) {
        new_text_brush_.remove_attribute(attr);
    }

    void clear_new_text_attributes() { new_text_brush_.clear_attributes(); }

    // Query Functions
    std::size_t row_length(std::size_t y) const;
    std::size_t display_height() const;

    std::size_t index_at(Point position) const {
        return this->index_at(position.x, position.y);
    }

    std::size_t index_at(std::size_t x, std::size_t y) const;
    Point display_position(std::size_t index) const;

    Glyph_string& contents() { return contents_; }
    const Glyph_string& contents() const { return contents_; }
    Glyph& glyph_at(std::size_t index) { return contents_.at(index); }
    Glyph glyph_at(std::size_t index) const { return contents_.at(index); }

    std::size_t contents_size() const { return contents_.size(); }
    bool contents_empty() const { return contents_.empty(); }
    bool does_word_wrap() const { return word_wrap_; }

    // Signals
    sig::Signal<void(std::size_t n)> scrolled_up;
    sig::Signal<void(std::size_t n)> scrolled_down;
    sig::Signal<void()> scrolled;
    sig::Signal<void(const Glyph_string&)> text_changed;

   protected:
    bool paint_event() override;

    std::size_t line_at(std::size_t index) const;

    std::size_t top_line() const { return top_line_; }

    std::size_t bottom_line() const {
        return this->top_line() + this->display_height() - 1;
    }

    std::size_t last_line() const { return display_state_.size() - 1; }

    std::size_t n_of_lines() const { return display_state_.size(); }

    std::size_t first_index_at(std::size_t line) const;

    std::size_t last_index_at(std::size_t line) const;

    std::size_t line_length(std::size_t line) const;

    std::size_t end_index() const {
        return this->contents_empty() ? 0 : this->contents_size() - 1;
    }

    void update_display(std::size_t from_line = 0);

   private:
    struct Line_info {
        std::size_t start_index;
        std::size_t length;
    };

    std::vector<Line_info> display_state_{Line_info{0, 0}};

    std::size_t top_line_{0};
    bool word_wrap_{true};
    Glyph_string contents_;
    Brush new_text_brush_{this->brush};  // TODO possibly make public member
    Alignment alignment_{Alignment::Left};
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_TEXT_DISPLAY_HPP
