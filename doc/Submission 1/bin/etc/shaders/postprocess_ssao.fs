#version 430

layout(location = 0) uniform sampler2D color_tex; 
layout(location = 1) uniform sampler2D depth_tex;
layout(location = 2) uniform sampler2D normal_tex;
layout(location = 3) uniform sampler2D pos_tex;

out vec4 fragColor;

uniform vec2 framebuffer_dim = vec2(1024, 768);

const float distanceThreshold = 10;
const float filterRadius = 0.005;
const int sample_count = 16;
const vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
                                vec2( -0.94201624,  -0.39906216 ),
                                vec2(  0.94558609,  -0.76890725 ),
                                vec2( -0.094184101, -0.92938870 ),
                                vec2(  0.34495938,   0.29387760 ),
                                vec2( -0.91588581,   0.45771432 ),
                                vec2( -0.81544232,  -0.87912464 ),
                                vec2( -0.38277543,   0.27676845 ),
                                vec2(  0.97484398,   0.75648379 ),
                                vec2(  0.44323325,  -0.97511554 ),
                                vec2(  0.53742981,  -0.47373420 ),
                                vec2( -0.26496911,  -0.41893023 ),
                                vec2(  0.79197514,   0.19090188 ),
                                vec2( -0.24188840,   0.99706507 ),
                                vec2( -0.81409955,   0.91437590 ),
                                vec2(  0.19984126,   0.78641367 ),
                                vec2(  0.14383161,  -0.14100790 )
                               );

// http://blog.evoserv.at/index.php/2012/12/hemispherical-screen-space-ambient-occlusion-ssao-for-deferred-renderers-using-openglglsl/
void main()	
{	
	vec2 tc 		= gl_FragCoord.xy / framebuffer_dim;
	vec3 color 	    = texture( color_tex,	tc).rgb;
	vec3 normal 	= texture( normal_tex,	tc).rgb;
	vec3 position 	= texture( pos_tex,	    tc).rgb;
	float depth 	= texture( depth_tex,	tc).r;
    
	float ambientOcclusion = 0;
	for (int i = 0; i < sample_count; ++i)
    {
        vec2 sample_tc = tc + (poisson16[i] * (filterRadius));
        vec3 samplePos = texture( pos_tex, sample_tc).rgb;
        vec3 sampleDir = normalize( samplePos - position );
		float d = distance(position, samplePos);
 
 // 
        float NdotS = max(dot(normal, sampleDir ) * 1/(d+1) , 0);
 
        float a = 1.0 - smoothstep(distanceThreshold, distanceThreshold * 2, d);
        float b = NdotS;
 
        ambientOcclusion += (a * b);
    }
	ambientOcclusion /= sample_count;
	
//	ambientOcclusion *= 5.0;
	
//	fragColor = vec4(color, 1.0f);
	fragColor = vec4(color-ambientOcclusion, 1.0f);
//	fragColor = vec4(color, 1.0f);
}