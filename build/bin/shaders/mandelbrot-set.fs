#version 330 core

#define AA 2
// #define COLORING

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 out_color;

uniform sampler2D u_texture;
uniform float time;
uniform float zoom;
uniform vec2 poi;

vec2 complex_product(vec2 a, vec2 b) {
    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

float f(vec2 uv) {
    vec2 z = vec2(0, 0);
    vec2 c = uv - poi;

    for (float i = 0; i < 256; i++) {
        z = complex_product(z, z) + c;
        if (dot(z, z) > 4) {
#ifdef COLORING
            float l = i;
            float sl = l - log2(log2(dot(z, z))) + 4.0;
            float al = smoothstep(-0.1, 0.0, 0.5 /*sin(0.5*6.2831*time ) */);
            l = mix(l, sl, al);

            return l;
#else
            return i / 256.0;
#endif
        }
    }
    return 1.0;
}

void main() {
    vec2 uv = (fragTexCoord * 2.0 - vec2(1));
    uv.x *= 1200.0 / 700.0;

#ifdef COLORING
    vec3 c = vec3(0.0);
    for (int k = 0; k < AA; k++) {
        for (int i = 0; i < AA; i++) {
            float a = f(uv / pow(zoom, 2));
            // a = sqrt(a);
            c += 0.5 + 0.5 * cos(3.0 + a * 0.15 + vec3(0.0, 0.0, 1.0));
        }
    }
    c /= float(AA * AA);
    out_color = vec4(c, 1);
#else
    float a = f(uv / (zoom * zoom));
    a = sqrt(a);
    out_color = vec4(a, a, a, 1);
#endif
    out_color = mix(texture(u_texture, fragTexCoord), out_color, 0.9);
}