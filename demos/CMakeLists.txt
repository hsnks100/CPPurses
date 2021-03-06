if(${CMAKE_VERSION} VERSION_LESS "3.8")
    set(CMAKE_CXX_STANDARD 14)
endif()

add_executable(demos EXCLUDE_FROM_ALL "")

# CHESS
# TODO can you include subdirectory instead?
target_sources(demos PRIVATE
    chess/src/chessboard_widget.cpp
    chess/src/position.cpp
    chess/src/chess_engine.cpp
    chess/src/rules.cpp
    chess/src/state.cpp
    chess/src/chessboard_borders.cpp
    chess/src/chess_ui.cpp
    chess/src/side_pane.cpp
    chess/src/settings_pane.cpp
    chess/src/lower_pane.cpp
    chess/src/move_input.cpp
    chess/src/piece.cpp
    chess/src/move.cpp
    chess/src/standard_rules.cpp
    chess/src/no_rules.cpp
    chess/src/shared_user_input.cpp
    chess/src/player_human.cpp
    chess/src/chess_event_loop.cpp
    chess/src/chess_move_request_event.cpp
    chess/src/shared_move.cpp
)

# GLYPH PAINT
target_sources(demos PRIVATE
    glyph_paint/glyph_paint.cpp
    glyph_paint/paint_area.cpp
    glyph_paint/side_pane.cpp
    glyph_paint/attribute_box.cpp
    glyph_paint/options_box.cpp
    glyph_paint/populated_glyph_stack.cpp
)

# ANIMATION
target_sources(demos PRIVATE
    animation/animated_widget.cpp
)

# FOCUS
target_sources(demos PRIVATE
    focus/focus_base.cpp
)

# PALETTE
target_sources(demos PRIVATE
    palette/palette_demo.cpp
    palette/value_control.cpp
    palette/color_definition_setter.cpp
    palette/color_control.cpp
    palette/palette_helpers.cpp
    palette/export_panel.cpp
    palette/color_display.cpp
    palette/all_colors_display.cpp
)

# NOTEPAD / MAIN MENU
target_sources(demos PRIVATE
    demo.cpp
    main_menu.cpp
    notepad/notepad.cpp
)

# GAME OF LIFE
target_sources(demos PRIVATE
    game_of_life/game_of_life_engine.cpp
    game_of_life/gol_widget.cpp
    game_of_life/gol_demo.cpp
    game_of_life/exporters.cpp
    game_of_life/filetype.cpp
    game_of_life/settings_box.cpp
    game_of_life/examples_info_box.cpp
    game_of_life/files_box.cpp
    game_of_life/offset_control_widget.cpp
    game_of_life/side_panel.cpp
    game_of_life/status_box.cpp
    game_of_life/get_rle.cpp
    game_of_life/get_life_1_05.cpp
    game_of_life/get_life_1_06.cpp
    game_of_life/get_plaintext.cpp
)

find_package(Threads REQUIRED)

target_link_libraries(demos PRIVATE cppurses ${CMAKE_THREAD_LIBS_INIT})

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.8")
    target_compile_features(demos INTERFACE cxx_std_14)
endif()
