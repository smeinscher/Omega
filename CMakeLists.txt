cmake_minimum_required(VERSION 3.28)
project(Omega C)

set(CMAKE_C_STANDARD 11)

find_package(glfw3 CONFIG REQUIRED)
find_package(cglm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(Omega
        external/glad/glad.h
        external/glad/glad.c
        external/stb/stb_image.h
        external/stb/stb_image.c
        src/main.c
        src/renderer/opengl_shader.h
        src/renderer/opengl_shader.c
        src/renderer/opengl_texture.h
        src/renderer/opengl_texture.c
        src/renderer/opengl_renderer.c
        src/renderer/opengl_renderer.h
        src/game/board.c
        src/game/board.h
        src/renderer/camera.c
        src/renderer/camera.h
        src/scenes/main_menu_scene.c
        src/scenes/main_menu_scene.h
        src/scenes/main_game_scene.c
        src/scenes/main_game_scene.h
        src/scenes/scene_state.c
        src/scenes/scene_state.h)

target_link_libraries(Omega
        PRIVATE
        cglm::cglm
        glfw
        ${OPENGL_LIBRARIES}
)

add_custom_target(copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
)

add_dependencies(Omega copy_resources)
