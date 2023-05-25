#version 150

uniform sampler2DRect tex0;
uniform float amplitude;
uniform vec3 flashColor;

in vec2 vTexCoord;

out vec4 fragColor;

void main() {
    vec4 color = texture(tex0, vTexCoord);
    float brightness = amplitude;
    color.rgb += brightness * flashColor;
    fragColor = color;
}

