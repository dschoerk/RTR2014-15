#version 430

layout(location = 0) uniform sampler2D color_tex;

uniform vec2 framebuffer_dim;
uniform float bloomCrop;

out vec4 fragColor;

const vec4 T = vec4(0.5,0.5,0.5,0.0);

void main(){
    vec2 tc 		= gl_FragCoord.xy / (framebuffer_dim / bloomCrop);
	fragColor = texture( color_tex, tc);

	//heavy bloom
//	fragColor.x = fragColor.x > T.x ? fragColor.x : 0.0;
//	fragColor.y = fragColor.y > T.y ? fragColor.y : 0.0;
//	fragColor.z = fragColor.z > T.z ? fragColor.z : 0.0;
//	fragColor.w = fragColor.w;

    //not so heavy bloom
	fragColor = max(fragColor-T,vec4(0));

}
