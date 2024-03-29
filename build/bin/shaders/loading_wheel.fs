#version 330 core

out vec4 resultColor;
uniform float iTime;
ivec2 iResolution = ivec2(1920, 1080);

#define SMOOTH(r) (mix(1.0, 0.0, smoothstep(0.9, 1.0, r)))
#define M_PI 3.1415926535897932384626433832795

float movingRing(vec2 uv, vec2 center, float r1, float r2) {
    vec2 d = uv - center;
    float r = sqrt(dot(d, d));
    d = normalize(d);
    float theta = -atan(d.y, d.x);
    theta = mod(-iTime + 0.5 * (1.0 + theta / M_PI), 1.0);
    // anti aliasing for the ring's head (thanks to TDM !)
    theta -= max(theta - 1.0 + 1e-2, 0.0) * 1e2;
    return theta * (SMOOTH(r / r2) - SMOOTH(r / r1));
}

void main() {
    vec2 uv = gl_FragCoord.xy;
    float ring = movingRing(uv, vec2(iResolution.x / 2.0, iResolution.y / 2.0), 20.0, 30.0);
    resultColor = vec4(ring);
}