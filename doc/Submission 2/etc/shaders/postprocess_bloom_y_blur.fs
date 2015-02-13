#version 430

layout(location = 0) uniform sampler2D color_tex;

out vec4 fragColor;

uniform float bloomCrop;
uniform vec2 framebuffer_dim;

uniform float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

/*uniform float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
uniform float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216,
                                   0.0540540541, 0.0162162162 );*/
								   
/*uniform float offset[16] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0);
uniform float weight[16] = float[]( 0.0997, 0.0967, 0.0880, 0.0753, 0.0605, 0.0457, 0.0324, 0.0216, 0.0135, 0.0079, 0.0044, 0.0023, 0.0011, 0.0005, 0.0002, 0.0001);*/

void main(void)
{
    const float pixelSize = bloomCrop / framebuffer_dim.y;
    const vec2 tc = gl_FragCoord.xy / (framebuffer_dim / bloomCrop);
    fragColor = texture2D( color_tex, tc) * weight[0];
    for (int i=1; i<3; i++) 
	{
        fragColor += texture2D( color_tex, ( tc+vec2(0.0, offset[i] * pixelSize) ) ) * weight[i];
        fragColor += texture2D( color_tex, ( tc-vec2(0.0, offset[i] * pixelSize) ) ) * weight[i];
    }
	//fragColor = texture2D( color_tex, tc);
}
