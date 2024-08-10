#version 460 core

out vec4 FragColor;

in vec2 uv;
in vec4 color;

// https://www.shadertoy.com/view/lslXDr
// scatter const
const float R_INNER = 1.0;
const float R = R_INNER + 0.5;

const int NUM_OUT_SCATTER = 8;
const int NUM_IN_SCATTER = 80;
vec3 in_scatter(vec3 o, vec3 dir, vec2 e, vec3 l)
{
    const float ph_ray = 0.05;
    const float ph_mie = 0.02;

    const vec3 k_ray = vec3(3.8, 13.5, 33.1);
    const vec3 k_mie = vec3(21.0);
    const float k_mie_ex = 1.1;

    vec3 sum_ray = vec3(0.0);
    vec3 sum_mie = vec3(0.0);

    float n_ray0 = 0.0;
    float n_mie0 = 0.0;

    float len = (e.y - e.x) / float(NUM_IN_SCATTER);
    vec3 s = dir * len;
    vec3 v = o + dir * (e.x + len * 0.5);

    for (int i = 0; i < NUM_IN_SCATTER; i++)
    {
        float d_ray = density(v, ph_ray) * len;
        float d_mie = density(v, ph_mie) * len;

        n_ray0 += d_ray;
        n_mie0 += d_mie;

        vec2 f = ray_vs_sphere(v, l, R);
        vec3 u = v + l * f.y;

        float n_ray1 = optic(v, u, ph_ray);
        float n_mie1 = optic(v, u, ph_mie);

        vec3 att = exp(-(n_ray0 + n_ray1) * k_ray - (n_mie0 + n_mie1) * k_mie * k_mie_ex);

        sum_ray += d_ray * att;
        sum_mie += d_mie * att;
        v += s;
    }

    float c = dot(dir, -l);
    float cc = c * c;
    vec3 scatter =
    sum_ray * k_ray * phase_ray(cc) +
    sum_mie * k_mie * phase_mie(-0.78, c, cc);


    return 10.0 * scatter;
}
float circle(vec2 st, float radius)
{
    vec2 dist = st - 0.5;
    return 1.0 - smoothstep(radius - (radius * 0.01), radius + (radius * 0.01), dot(dist, dist));
}

void main()
{
    vec3 eye = vec3(0.0, 0.0, 3.0);
    vec3 l = vec3(0.0, 0.0, 1.0);
    vec4 col = vec4(circle(uv, 0.08));
    //    circle.x *= 74.0 / 64.0;
    //
    //    circle = circle * 2.0 - 1.0;
    //    float d = length(abs(circle) - 0.3);
    //    vec3 col = vec3(step(0.0, d));
    vec3 I = in_scatter(eye, )
    FragColor = col;// * color;// * texture(texture1, uv);
}