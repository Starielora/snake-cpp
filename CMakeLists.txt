cmake_minimum_required(VERSION 3.22)

project(snake-cpp)

    add_subdirectory(deps)

    add_executable(snake-cpp
        # main.cpp
        glfwmain.cpp
        game_logic.hpp
        stdout_render.hpp
        windows_console_input.hpp
        bot.hpp
        console_windows.hpp
        console_linux.hpp

        camera.hpp
        openglutils.hpp
        shader.hpp
    )

    set_target_properties(snake-cpp PROPERTIES CXX_STANDARD 20)

    target_link_libraries(snake-cpp PRIVATE glm::glm glfw glad)
