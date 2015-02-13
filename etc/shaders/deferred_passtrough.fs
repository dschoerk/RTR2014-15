#version 430
#define EPSILON 0.005

const float f = 3000; // hardcoded light projection parameters
const float n = 1;

layout(location = 0) uniform sampler2D worldPos_tex;
layout(location = 1) uniform sampler2D diffuse_tex;
layout(location = 2) uniform sampler2D normal_tex;
layout(location = 3) uniform sampler2D spec_tex;
layout(location = 4) uniform sampler2D alpha_tex;
layout(location = 5) uniform sampler2D depth_tex;

layout(location = 12) uniform samplerCube shadowTex;

out vec4 fragColor;

uniform vec3 lightpos;
uniform vec3 cameraPos;
uniform vec3 lightcolor;
uniform vec2 framebuffer_dim;
uniform bool lightingOn = true;

float linstep(float low, float high, float v)
{
    return clamp((v-low)/(high-low), 0.0, 1.0);
}

void main()
{
	vec2 tc 		= gl_FragCoord.xy / framebuffer_dim;
	vec3 pos 		= texture( worldPos_tex,tc).rgb;
	vec3 normal 	= texture( normal_tex,	tc).rgb;
	vec3 diffuse 	= texture( diffuse_tex,	tc).rgb ;
	float shininess = texture( spec_tex,	tc).rgb;
	float alpha		= texture( alpha_tex,	tc).r;
	float depth 	= texture( depth_tex,	tc).r;
	normal = normalize(normal);


	if(lightingOn)
	{
		vec3 lightDirection =     lightpos - pos;
		vec3 nLightDirection =    normalize(lightDirection);
		vec3 vAmbientLight		= lightcolor * vec3(0.5f);
		vec3 vDiffuseLight		= lightcolor * dot(normal, nLightDirection);
		
		vec3 eyeDirection = normalize(cameraPos - pos);
		vec3 LightReflect = normalize(reflect(-nLightDirection, normal));
		float cosAngle = max(0.0, dot(eyeDirection, LightReflect));
		float SpecularFactor = pow(cosAngle, 3) * shininess * 0.8;
		vec3 vSpecularLight = lightcolor * max(SpecularFactor, 0.0);

		// <shadowmapping>
		float intensity = 1.0;
		float distance = (distance(lightpos, pos)-n) / (f-n);
		vec2 moments = texture(shadowTex, nLightDirection).rg;
		if (moments.x < distance - 0.001)//*tan(acos(dot(normal, nLightDirection))))
		{
			float p = smoothstep(distance-0.02, distance, moments.x);
			float variance = max(moments.y - (moments.x * moments.x), -0.001);
			float d = distance - moments.x;
			float p_max = linstep(0.2, 1.0, variance / (variance + d*d));
			intensity = clamp(max(p, p_max), 0.0, 1.0);
		}
		
		// </shadowmapping>

    	fragColor = vec4(((vAmbientLight + vDiffuseLight)*diffuse + SpecularFactor) *max(intensity, 0.05),alpha);
//		fragColor = vec4(diffuse / 4, 1);
	}
	else
	{
		fragColor = vec4(diffuse,alpha); // alpha was wrong because alpha tex of plants is rendered always
	}
}
