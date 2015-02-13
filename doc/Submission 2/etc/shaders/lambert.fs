#version 330

in vec3 normal;
out vec4 fragColor;

uniform vec3 lightdir;

void main()
{
    fragColor = dot(normal, lightdir) * vec4(1,1,1,1);
}