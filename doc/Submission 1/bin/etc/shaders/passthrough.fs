#version 330

in vec3 color;
in vec3 normal;
in vec3 lightDir;
out vec4 fragColor;
uniform float time;


void main()
{
	float PI = 3.14159265358979323846264;
	float t = (sin(2*PI*mod(time/3, 1.0))+1) / 2;
	float I = dot(normalize(lightDir), normal);
    fragColor = vec4(vec3(I), 1);
}