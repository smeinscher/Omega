#version 460 core

out vec4 FragColor;

in vec4 color;
in float t;

#define FAR 400.0

const vec3 lgt = vec3(-.523, .41, -.747);

// https://www.shadertoy.com/view/XsyGWV
float linstep(in float mn, in float mx, in float x)
{
    return clamp((x - mn) / (mx - mn), 0., 1.);
}

vec3 scatter(vec3 ro, vec3 rd)
{
    float sd = max(dot(lgt, rd) * 0.5 + 0.5, 0.);
    float dtp = 13. - (ro + rd * (FAR)).y * 3.5;
    float hori = (linstep(-1500., 0.0, dtp) - linstep(11., 500., dtp)) * 1.;
    hori *= pow(sd, .04);

    vec3 col = vec3(0);
    col += pow(hori, 200.) * vec3(1.0, 0.7, 0.5) * 3.;
    col += pow(hori, 25.) * vec3(1.0, 0.5, 0.25) * .3;
    col += pow(hori, 7.) * vec3(1.0, 0.4, 0.25) * .8;

    return col;
}

vec3 nmzHash33(vec3 q)
{
    uvec3 p = uvec3(ivec3(q));
    p = p * uvec3(374761393U, 1103515245U, 668265263U) + p.zxy + p.yzx;
    p = p.yzx * (p.zxy ^ (p >> 3U));
    return vec3(p ^ (p >> 16U)) * (1.0 / vec3(0xffffffffU));
}

vec3 stars(vec3 p)
{
    vec3 c = vec3(0.0);
    float res = 1280.0 / 720.0;
    for (int i = 0; i < 3; i++)
    {
        vec3 q = fract(p * (0.15 * res)) - 0.5;
        vec3 id = floor(p * (0.15 * res));
        vec2 rn = nmzHash33(id).xy;
        float c2 = 1.0 - smoothstep(0.0, 0.6, length(q));
        c2 *= step(rn.x, 0.0005 + i * i * 0.001);
        c += c2 * (mix(vec3(1.0, 0.49, 0.1), vec3(0.75, 0.9, 1.0), rn.y) * 0.25 + 0.75);
        p *= 1.4;
    }
    return c * c * 0.7;
}

void main()
{
    vec2 res = vec2(2.0, 2.0);
    vec2 q = gl_FragCoord.xy / res;

    vec3 bg = stars(vec3(q + t * 10.0, 1.0));

    FragColor = vec4(bg, 1.0) * color;
}