#version 430

layout(location = 0) uniform sampler2D color_tex;
uniform float bloomCrop;
uniform vec2 framebuffer_dim = vec2(1024, 768);

out vec4 fragColor;

const float[5] kernel = float[](0.061f, 0.242f, 0.383f, 0.242f, 0.061f);

void main(){
    vec2 tc = gl_FragCoord.xy / (framebuffer_dim / bloomCrop);
    float pixelSize = bloomCrop / framebuffer_dim.y;
	fragColor  = kernel[0] * texture( color_tex, vec2(tc.x,tc.y-2.0f*pixelSize));
	fragColor += kernel[1] * texture( color_tex, vec2(tc.x,tc.y-1.0f*pixelSize));
	fragColor += kernel[2] * texture( color_tex, tc);
	fragColor += kernel[3] * texture( color_tex, vec2(tc.x,tc.y+1.0f*pixelSize));
	fragColor += kernel[4] * texture( color_tex, vec2(tc.x,tc.y+2.0f*pixelSize));
}