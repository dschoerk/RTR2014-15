#version 440

out vec2 fragColor;
in vec3 wpos;
in vec4 vpos;
uniform vec3 lightpos;

const float f = 3000;
const float n = 1;

void main()
{
	float d = (distance(lightpos,wpos)-n)/(f-n);
	//d = d * 0.5 + 0.5;
	// 1. Moment
    fragColor.x = d;

	// 2. Moment
	float dx = dFdx(d);
	float dy = dFdy(d);
	fragColor.y = d * d + 0.25 * ( dx * dx + dy * dy) ;
}
