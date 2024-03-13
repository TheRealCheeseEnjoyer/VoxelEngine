#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D TexId;

void main() {
    FragColor = texture(TexId, TexCoord);
}