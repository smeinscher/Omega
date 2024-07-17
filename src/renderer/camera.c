//
// Created by sterling on 7/5/24.
//

#include "camera.h"
#include <cglm/cglm.h>

static int g_viewport_width = 1280;
static int g_viewport_height = 720;

static vec3 g_camera_position = {0.0f, 0.0f, 2.0f};

static bool g_camera_moving_up = false;
static bool g_camera_moving_down = false;
static bool g_camera_moving_left = false;
static bool g_camera_moving_right = false;
static float g_camera_speed = 3.0f;

static float g_zoom_factor = 100.0f;
static float g_zoom = 100.0f;

static mat4 g_view;
static mat4 g_projection;

void camera_update()
{
    vec3 velocity = {0.0f, 0.0f, 0.0f};
    if (g_camera_moving_up)
    {
        velocity[1] -= 1.0f;
    }
    if (g_camera_moving_down)
    {
        velocity[1] += 1.0f;
    }
    if (g_camera_moving_left)
    {
        velocity[0] -= 1.0f;
    }
    if (g_camera_moving_right)
    {
        velocity[0] += 1.0f;
    }
    glm_vec3_norm(velocity);
    glm_vec3_scale(velocity, g_camera_speed, velocity);
    glm_vec3_scale(velocity, camera_get_zoom_ratio(), velocity);
    glm_vec3_add(g_camera_position, velocity, g_camera_position);

    vec3 forward = {0.0f, 0.0f, -1.0f};
    vec3 up = {0.0f, 1.0f, 0.0f};
    vec3 camera_position_plus_forward;
    glm_vec3_add(g_camera_position, forward, camera_position_plus_forward);
    glm_lookat(g_camera_position, camera_position_plus_forward, up, g_view);

    float left = (float)g_viewport_width - (float)g_viewport_width / (g_zoom_factor / g_zoom);
    float right = (float)g_viewport_width / (g_zoom_factor / g_zoom);
    float bottom = (float)g_viewport_height / (g_zoom_factor / g_zoom);
    float top = (float)g_viewport_height - (float)g_viewport_height / (g_zoom_factor / g_zoom);
    glm_ortho(left, right, bottom, top, 0.1f, 10.0f, g_projection);
}

void camera_set_moving_up(bool moving_up)
{
    g_camera_moving_up = moving_up;
}

void camera_set_moving_down(bool moving_down)
{
    g_camera_moving_down = moving_down;
}

void camera_set_moving_left(bool moving_left)
{
    g_camera_moving_left = moving_left;
}

void camera_set_moving_right(bool moving_right)
{
    g_camera_moving_right = moving_right;
}

float camera_get_zoom()
{
    return g_zoom;
}

float camera_get_zoom_factor()
{
    return g_zoom_factor;
}

float camera_get_zoom_ratio()
{
    return g_zoom_factor / g_zoom;
}

void camera_set_zoom(float zoom)
{
    g_zoom = zoom;
}

void camera_increment_zoom(float amount)
{
    g_zoom += amount;
}

void camera_decrement_zoom(float amount)
{
    g_zoom -= amount;
}

vec3 *camera_get_position()
{
    return (vec3 *)g_camera_position;
}

void camera_set_position(const vec3 position)
{
    g_camera_position[0] = position[0];
    g_camera_position[1] = position[1];
    g_camera_position[2] = position[2];
}

mat4 *camera_get_projection()
{
    return (mat4 *)g_projection;
}

mat4 *camera_get_view()
{
    return (mat4 *)g_view;
}

int camera_get_viewport_width()
{
    return g_viewport_width;
}

int camera_get_viewport_height()
{
    return g_viewport_height;
}
