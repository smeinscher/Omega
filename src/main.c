#include "../external/glad/glad.h"
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL4_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "../external/nuklear/nuklear.h"
#include "../external/nuklear/nuklear_glfw_gl4.h"
#include "renderer/opengl_renderer.h"
#include "scenes/scene_state.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define MAX_VERTEX_BUFFER (512 * 1024)
#define MAX_ELEMENT_BUFFER (128 * 1024)

#define TIME_PER_UPDATE (1.0 / 60.0)

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My Game", NULL, NULL);
    if (window == NULL)
    {
        // TODO: logging stuff
        printf("Error: failed to create window");
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        // TODO: logging stuff
        printf("Error: failed to initiate glad");
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_WIDTH);
    glLineWidth(1.0f);
    glfwSwapInterval(1);

    scene_set(MAIN_GAME, window);

    struct nk_context *ctx;
    struct nk_colorf bg;
    struct nk_image img;

    ctx = nk_glfw3_init(window, NK_GLFW3_DEFAULT, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();

    int tex_index = 0;
    enum
    {
        tex_width = 256,
        tex_height = 256
    };
    char pixels[tex_width * tex_height * 4];
    memset(pixels, 128, sizeof(pixels));
    tex_index = nk_glfw3_create_texture(pixels, tex_width, tex_height);
    img = nk_image_id(tex_index);

    bg.r = 0.1f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

    double previous_update_time = 0.0;
    double lag = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        double current_update_time = glfwGetTime();
        double elapsed_time = current_update_time - previous_update_time;
        previous_update_time = current_update_time;
        lag += elapsed_time;
        while (lag >= TIME_PER_UPDATE)
        {
            scene_update();
            lag -= TIME_PER_UPDATE;
        }
        scene_render();
        nk_glfw3_new_frame();
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
                     NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE |
                         NK_WINDOW_TITLE))
        {
            nk_label(ctx, "Cool", NK_TEXT_LEFT);
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "Button"))
            {
                printf("Test");
            }
        }
        nk_end(ctx);
        nk_glfw3_render(NK_ANTI_ALIASING_ON);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_LINE_WIDTH);
        glLineWidth(1.0f);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean_all_vertex_data();
    nk_glfw3_shutdown();
    glfwTerminate();
    return 0;
}
