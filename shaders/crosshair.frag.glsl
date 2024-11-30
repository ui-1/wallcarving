#version 400

in vec3 fragColor;  // Color from vertex shader
out vec4 outColor;  // Final color output

void main() {
    outColor = vec4(fragColor, 1.0);  // Set the final fragment color
}
