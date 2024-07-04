cmake_minimum_required(VERSION 3.28)
project(Omega C)

set(CMAKE_C_STANDARD 11)

find_package(glfw3 CONFIG REQUIRED)
find_package(cglm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(Omega
        external/glad/glad.h
        external/glad/glad.c
        external/stb/stb_image.h
        external/stb/stb_image.c
        main.c
        renderer/opengl_shader.h
        renderer/opengl_shader.c
        renderer/opengl_texture.h
        renderer/opengl_texture.c
        renderer/opengl_renderer.c
        renderer/opengl_renderer.h
        game/board.c
        game/board.h)

target_link_libraries(Omega
        PRIVATE
        cglm::cglm
        imgui::imgui
        glfw
        ${OPENGL_LIBRARIES}
)

add_custom_target(copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
)

add_dependencies(Omega copy_resources)
