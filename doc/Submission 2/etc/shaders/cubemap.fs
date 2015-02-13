#version 330 core

in vec3 fragmentUV;
uniform samplerCube cubeTexture;
out vec4 fragColor;

void main () {
  fragColor = texture(cubeTexture, fragmentUV);
}