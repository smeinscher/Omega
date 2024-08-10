#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 2) in vec4 aColor;

out vec4 color;
out float t;

uniform float time;

void main()
{
    gl_Position = vec4(aPos, 1.0f, 1.0f);
    color = aColor;
    t = time;
}