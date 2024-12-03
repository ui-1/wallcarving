#version 400

layout(location = 0) in vec2 position;  // Vertex position
layout(location = 1) in vec3 color;     // Vertex color

out vec3 fragColor;  // Output color to fragment shader

void main() {
    fragColor = color;  // Pass color to fragment shader
    gl_Position = vec4(position, 0.0, 1.0);  // 2D coordinates (no depth, z = 0.0)
}
