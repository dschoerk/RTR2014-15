#version 330

layout(location = 0) in vec4 in_position;
layout(location = 1) in vec3 in_normal;

out vec3 normal;

void main()
{
	normal = in_normal;
    gl_Position = in_position;
}