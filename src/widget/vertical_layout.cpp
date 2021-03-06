#include <cppurses/widget/layouts/vertical_layout.hpp>

#include <cstddef>
#include <deque>
#include <iterator>
#include <vector>

#include <cppurses/system/events/move_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

std::vector<Layout::Dimensions> Vertical_layout::calculate_widget_sizes() {
    std::vector<Dimensions> widgets;
    std::size_t total_stretch{0};
    for (const std::unique_ptr<Widget>& c : this->children.get()) {
        if (c->enabled()) {  // TODO do not include Ignored height policy widgs?
            // widgets.emplace_back(Dimensions{c.get(), 0, 0});
            widgets.emplace_back(Dimensions{c.get(), this->width(), 0});
            total_stretch += c->height_policy.stretch();
        }
    }

    int height_available = this->height();

    // VERTICAL
    // Set Fixed, Minimum and MinimumExpanding to height_hint
    for (Dimensions& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Fixed || policy == Size_policy::Minimum ||
            policy == Size_policy::MinimumExpanding) {
            d.height = d.widget->height_policy.hint();
            height_available -= d.height;
        }
    }
    // if (height_available < 0) {
    //     return widgets;
    // }

    // Set Size_policy::Ignored widgets to their stretch factor height value
    for (Dimensions& d : widgets) {
        if (d.widget->height_policy.type() == Size_policy::Ignored) {
            const float percent = d.widget->height_policy.stretch() /
                                  static_cast<float>(total_stretch);
            std::size_t height = percent * this->height();
            if (height < d.widget->height_policy.min()) {
                height = d.widget->height_policy.min();
            } else if (height > d.widget->height_policy.max()) {
                height = d.widget->height_policy.max();
            }
            d.height = height;
            height_available -= height;
        }
    }

    // Set Maximum, Preferred and Expanding to height_hint
    for (Dimensions& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Expanding) {
            d.height = d.widget->height_policy.hint();
            height_available -= d.height;
        }
    }

    /// DISTRIBUTE SPACE ------------------------------------------------------

    // create vector of size references for below if statements
    std::vector<Dimensions_reference> widgets_w_refs;
    widgets_w_refs.reserve(widgets.size());
    for (Dimensions& d : widgets) {
        widgets_w_refs.emplace_back(
            Dimensions_reference{d.widget, &d.width, &d.height});
    }
    // If space left, fill in expanding and min_expanding, then if still,
    // preferred and min
    if (height_available > 0) {
        this->distribute_space(widgets_w_refs, height_available);
    }

    // if negative space left, subtract from max and preferred, then if still
    // needed, expanding
    if (height_available < 0) {
        this->collect_space(widgets_w_refs, height_available);
    }

    /// DISTRIBUTE SPACE ------------------------------------------------------

    // HORIZONTAL - repeat the above, but with horizontal properties
    for (Dimensions& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Fixed) {
            d.width = d.widget->width_policy.hint();
        } else if (policy == Size_policy::Ignored ||
                   policy == Size_policy::Preferred ||
                   policy == Size_policy::Expanding) {
            if (d.width > d.widget->width_policy.max()) {
                d.width = d.widget->width_policy.max();
            } else if (d.width < d.widget->width_policy.min()) {
                d.width = d.widget->width_policy.min();
            }
        } else if (policy == Size_policy::Maximum) {
            if (d.width > d.widget->width_policy.hint()) {
                d.width = d.widget->width_policy.hint();
            }
        } else if (policy == Size_policy::Minimum ||
                   policy == Size_policy::MinimumExpanding) {
            if (d.width > d.widget->width_policy.max()) {
                d.width = d.widget->width_policy.max();
            } else if (d.width < d.widget->width_policy.hint()) {
                d.width = d.widget->width_policy.hint();
            }
        }
    }
    return widgets;
}

void Vertical_layout::distribute_space(
    std::vector<Dimensions_reference> widgets,
    int height_left) {
    // Find total stretch of first group
    std::size_t total_stretch{0};
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            total_stretch += d.widget->height_policy.stretch();
        }
    }

    // Calculate new heights of widgets in new group, if any go over max_height
    // then assign max value and recurse without that widget in vector.
    std::deque<std::size_t> height_additions;
    int index{0};
    auto to_distribute = height_left;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            height_additions.push_back((d.widget->height_policy.stretch() /
                                        static_cast<double>(total_stretch)) *
                                       to_distribute);
            if ((*d.height + height_additions.back()) >
                d.widget->height_policy.max()) {
                height_left += d.widget->height_policy.max() - *d.height;
                *d.height = d.widget->height_policy.max();
                widgets.erase(std::begin(widgets) + index);
                return distribute_space(widgets, height_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            *d.height += height_additions.front();
            height_left -= height_additions.front();
            height_additions.pop_front();
        }
    }

    // SECOND GROUP - duplicate of above dependent on Policies to work with.
    // Preferred and Minimum
    if (height_left == 0) {
        return;
    }
    // Find total stretch
    total_stretch = 0;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            total_stretch += d.widget->height_policy.stretch();
        }
    }

    // Calculate new heights of widgets in new group, if any go over max_height
    // then assign max value and recurse without that widget in vector.
    height_additions.clear();
    index = 0;
    to_distribute = height_left;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            height_additions.push_back((d.widget->width_policy.stretch() /
                                        static_cast<double>(total_stretch)) *
                                       to_distribute);
            if ((*d.height + height_additions.back()) >
                d.widget->height_policy.max()) {
                height_left -= d.widget->height_policy.max() - *d.height;
                *d.height = d.widget->height_policy.max();
                widgets.erase(std::begin(widgets) + index);
                return distribute_space(widgets, height_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            *d.height += height_additions.front();
            height_left -= height_additions.front();
            height_additions.pop_front();
        }
    }
    if (height_left == 0) {
        return;
    }
    // Rounding error extra
    // First Group
    auto height_check{0};
    do {
        height_check = height_left;
        for (Dimensions_reference& d : widgets) {
            auto policy = d.widget->height_policy.type();
            if ((policy == Size_policy::Expanding ||
                 policy == Size_policy::MinimumExpanding) &&
                height_left > 0) {
                if (*d.height + 1 <= d.widget->height_policy.max()) {
                    *d.height += 1;
                    height_left -= 1;
                }
            }
        }
    } while (height_check != height_left);

    // Second Group
    do {
        height_check = height_left;
        for (Dimensions_reference& d : widgets) {
            auto policy = d.widget->height_policy.type();
            if ((policy == Size_policy::Preferred ||
                 policy == Size_policy::Minimum ||
                 policy == Size_policy::Ignored) &&
                height_left > 0) {
                if (*d.height + 1 <= d.widget->height_policy.max()) {
                    *d.height += 1;
                    height_left -= 1;
                }
            }
        }
    } while (height_check != height_left);
}

void Vertical_layout::collect_space(std::vector<Dimensions_reference> widgets,
                                    int height_left) {
    if (height_left == 0) {
        return;
    }
    // Find total stretch of first group
    std::size_t total_stretch{0};
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            total_stretch += d.widget->height_policy.stretch();
        }
    }

    // Find total of inverse of percentages
    double total_inverse{0};
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            total_inverse += 1 / (d.widget->height_policy.stretch() /
                                  static_cast<double>(total_stretch));
        }
    }

    // Calculate new heights of widgets in new group, if any go under min_height
    // then assign min value and recurse without that widget in vector.
    std::deque<std::size_t> height_deductions;
    int index{0};
    auto to_collect = height_left;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            height_deductions.push_back(
                ((1 / (d.widget->height_policy.stretch() /
                       static_cast<double>(total_stretch))) /
                 static_cast<double>(total_inverse)) *
                (to_collect * -1));
            if ((*d.height - height_deductions.back()) <
                d.widget->height_policy.min()) {
                height_left -= *d.height - d.widget->height_policy.min();
                *d.height = d.widget->height_policy.min();
                widgets.erase(std::begin(widgets) + index);
                return collect_space(widgets, height_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            if (*d.height >= height_deductions.front()) {
                *d.height -= height_deductions.front();
            } else {
                *d.height = 0;
            }
            height_left += height_deductions.front();
            height_deductions.pop_front();
        }
    }

    // SECOND GROUP - duplicate of above dependent on Policies to work with.
    if (height_left == 0) {
        return;
    }
    // Find total stretch
    total_stretch = 0;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Expanding) {
            total_stretch += d.widget->height_policy.stretch();
        }
    }

    // Find total of inverse of percentages
    total_inverse = 0;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Expanding) {
            total_inverse += 1 / (d.widget->height_policy.stretch() /
                                  static_cast<double>(total_stretch));
        }
    }

    // Calculate new heights of widgets in new group, if any go over max_height
    // then assign max value and recurse without that widget in vector.
    height_deductions.clear();
    index = 0;
    to_collect = height_left;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Expanding) {
            height_deductions.push_back(
                ((1 / (d.widget->width_policy.stretch() /
                       static_cast<double>(total_stretch))) /
                 static_cast<double>(total_inverse)) *
                (to_collect * -1));
            if ((*d.height - height_deductions.back()) <
                d.widget->height_policy.min()) {
                height_left += *d.height - d.widget->height_policy.min();
                *d.height = d.widget->height_policy.min();
                widgets.erase(std::begin(widgets) + index);
                return collect_space(widgets, height_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (Dimensions_reference& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Expanding) {
            if (*d.height >= height_deductions.front()) {
                *d.height -= height_deductions.front();
            } else {
                *d.height = 0;
            }
            height_left += height_deductions.front();
            height_deductions.pop_front();
        }
    }
    // TODO Change this to distribute the space, it might not be too small
    if (height_left != 0) {
        return;
    }
}

void Vertical_layout::move_and_resize_children(
    const std::vector<Dimensions>& dimensions) {
    const std::size_t parent_x{this->inner_x()};
    const std::size_t parent_y{this->inner_y()};
    const std::size_t parent_height{this->height()};
    const std::size_t parent_width{this->width()};
    std::size_t y_pos{parent_y};
    for (const Dimensions& d : dimensions) {
        if ((y_pos + d.height) > (parent_y + parent_height) ||
            (parent_x + d.width) > (parent_x + parent_width) || d.height == 0 ||
            d.width == 0) {
            // TODO this allows a large widget in the middle to disapear and
            // smaller widgets from the end to reappear once that happens. Maybe
            // you should stop sending events to any other widget once you get
            // here and disable all that are left.
            d.widget->disable(true, false);  // don't send child_polished_events
        } else {
            System::post_event<Move_event>(*(d.widget), Point{parent_x, y_pos});
            System::post_event<Resize_event>(*(d.widget),
                                             Area{d.width, d.height});
            y_pos += d.height;
        }
    }
}

void Vertical_layout::update_geometry() {
    this->enable(true, false);
    std::vector<Dimensions> heights{this->calculate_widget_sizes()};
    this->move_and_resize_children(heights);
}

}  // namespace cppurses
