cmake_minimum_required(VERSION 3.22)

project(snake-cpp)

    add_executable(snake-cpp
        main.cpp
        game_logic.hpp
        stdout_render.hpp
        windows_console_input.hpp
        vec2i.h
        bot.hpp
        console_windows.hpp
        console_linux.hpp
    )

    set_target_properties(snake-cpp PROPERTIES CXX_STANDARD 20)
