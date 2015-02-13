#version 430

layout(location = 0) uniform sampler2D startTexture;
layout(location = 1) uniform sampler2D bloomTexture;

uniform vec2 framebuffer_dim;

out vec4 fragColor;

void main(){
    vec2 tc = gl_FragCoord.xy / framebuffer_dim;
	vec4 src  = texture( startTexture, tc);
	vec4 dest =	texture( bloomTexture, tc);
	
	
	fragColor  = src+dest;

	//color = (src + dest) - (src * dest);

	//color = smoothstep(src,vec3(1),(src+dest));
}