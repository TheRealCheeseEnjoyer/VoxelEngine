#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
uniform mat4 stuff;

void main() {
    gl_Position = stuff * vec4(aPos, 1);
    TexCoord = aTexCoord;
}