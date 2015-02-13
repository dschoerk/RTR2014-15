#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;
out vec3 color;
out vec3 normal;
out vec3 lightDir;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform vec3 lightPos;

void main()
{
	color = in_color;
	lightDir = lightPos - vec3(model * vec4(in_position, 1));
	normal = normalMatrix * in_normal;
    gl_Position = projection * view * model * vec4(in_position, 1);
}