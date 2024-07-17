cmake_minimum_required(VERSION 3.28)
project(OmegaCradle C)
project(OmegaApp C)

set(CMAKE_C_STANDARD 11)

find_package(glfw3 CONFIG REQUIRED)
find_package(cglm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(OmegaCradle src/main.c)
add_library(OmegaApp SHARED
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
        src/objects/board.c
        src/objects/board.h
        src/renderer/camera.c
        src/renderer/camera.h
        src/scenes/main_menu_scene.c
        src/scenes/main_menu_scene.h
        src/scenes/main_game_scene.c
        src/scenes/main_game_scene.h
        src/scenes/scene_state.c
        src/scenes/scene_state.h
        src/ui/nuklear_config.c
        src/ui/nuklear_config.h
        src/game/game.c
        src/game/game.h
        src/platform/glfw_platform.c
        src/platform/platform.h
        src/util/file_helpers.c
        src/util/file_helpers.h
        src/game/loader.c
        src/game/loader.h)

target_link_libraries(OmegaApp
        PRIVATE
        cglm::cglm
        glfw
        ${OPENGL_LIBRARIES}
)

target_link_libraries(OmegaCradle OmegaApp)
