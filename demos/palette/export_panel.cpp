#include "export_panel.hpp"

#include <cctype>
#include <fstream>
#include <string>

#include <cppurses/painter/rgb.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget_free_functions.hpp>

namespace {
std::string rgb_to_str(const cppurses::RGB& rgb) {
    std::string result{"{"};
    result += std::to_string(rgb.red);
    result += ", ";
    result += std::to_string(rgb.green);
    result += ", ";
    result += std::to_string(rgb.blue);
    result += "}";
    return result;
}

/// Overwrites name.hpp with functions returning the current palette.
void export_current_palette(const std::string& name) {
    auto file = std::ofstream{name + ".hpp"};
    auto pal = cppurses::System::terminal.current_palette();

    file << R"(#include <cppurses/painter/color.hpp>)" << '\n'
         << R"(#include <cppurses/painter/palette.hpp>)"
         << "\n\n"
         << "cppurses::Palette " << name << "() {" << '\n';

    const auto prefix = std::string{"             {cppurses::Color::"};
    file << "    return {{{cppurses::Color::Black, "
         << rgb_to_str(pal[0].values) << "},\n"
         << prefix << "Dark_red, " << rgb_to_str(pal[1].values) << "},\n"
         << prefix << "Dark_blue, " << rgb_to_str(pal[2].values) << "},\n"
         << prefix << "Dark_gray, " << rgb_to_str(pal[3].values) << "},\n"
         << prefix << "Brown, " << rgb_to_str(pal[4].values) << "},\n"
         << prefix << "Green, " << rgb_to_str(pal[5].values) << "},\n"
         << prefix << "Red, " << rgb_to_str(pal[6].values) << "},\n"
         << prefix << "Gray, " << rgb_to_str(pal[7].values) << "},\n"
         << prefix << "Blue, " << rgb_to_str(pal[8].values) << "},\n"
         << prefix << "Orange, " << rgb_to_str(pal[9].values) << "},\n"
         << prefix << "Light_gray, " << rgb_to_str(pal[10].values) << "},\n"
         << prefix << "Light_green, " << rgb_to_str(pal[11].values) << "},\n"
         << prefix << "Violet, " << rgb_to_str(pal[12].values) << "},\n"
         << prefix << "Light_blue, " << rgb_to_str(pal[13].values) << "},\n"
         << prefix << "Yellow, " << rgb_to_str(pal[14].values) << "},\n"
         << prefix << "White, " << rgb_to_str(pal[15].values) << "}}};\n"
         << "}\n";
}

}  // namespace

namespace palette {

Export_panel::Export_panel() {
    using namespace cppurses;

    this->height_policy.type(Size_policy::Fixed);
    this->height_policy.hint(3);

    this->border.enabled = true;
    disable_walls(this->border);
    disable_corners(this->border);
    this->border.north_enabled = true;

    name_edit_.brush.set_background(Color::Dark_gray);
    name_edit_.set_ghost_color(Color::Light_gray);
    name_edit_.set_validator(
        [](char c) { return !std::iscntrl(c) || !std::isspace(c); });

    export_btn_.brush.set_background(Color::White);
    export_btn_.brush.set_foreground(Color::Black);
    export_btn_.clicked.connect(
        [this]() { export_current_palette(name_edit_.contents().str()); });
}
}  // namespace palette
