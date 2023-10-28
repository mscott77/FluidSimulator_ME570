#version 330 core

// vertex shader

// input:  attribute named 'position' with 3 floats per vertex
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec4 fragColor;

uniform mat4 transform;

void main() {
  gl_Position = transform * vec4(position, 1.0f);
  fragColor = vec4(color, 1.0);
}
