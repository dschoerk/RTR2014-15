#version 430

layout(location = 0) uniform sampler2D worldPos_tex; 
layout(location = 1) uniform sampler2D diffuse_tex;
layout(location = 2) uniform sampler2D normal_tex;
layout(location = 3) uniform sampler2D spec_tex;
layout(location = 4) uniform sampler2D alpha_tex;
layout(location = 5) uniform sampler2D depth_tex;

out vec4 fragColor;

uniform vec3 lightpos;
uniform vec3 lightcolor;
uniform vec2 framebuffer_dim;

uniform bool lightingOn = true;

void main()	
{	
	vec2 tc 		= gl_FragCoord.xy / framebuffer_dim;
	vec3 pos 		= texture( worldPos_tex,tc).rgb;
	vec3 normal 	= texture( normal_tex,	tc).rgb;
	vec3 diffuse 	= texture( diffuse_tex,	tc).rgb ;
	float shininess = texture( spec_tex,	tc).a*255.0;
	float alpha		= texture( alpha_tex,	tc).r;
	float depth 	= texture( depth_tex,	tc).r;
	normal = normalize(normal);
    
	if(lightingOn)
	{
		vec3 lightDirection = normalize(lightpos-pos);

		vec3 vAmbientLight		= lightcolor * vec3(0.8f);
		vec3 vDiffuseLight		= lightcolor * dot(normal, lightDirection);
		vec3 vSpecularLight		= vec3(0.0f);
		
		if (shininess < 255.0) {
			vec3 eyeDirection = normalize(-pos);
			vec3 LightReflect = normalize(reflect(-lightDirection, normal));
			float SpecularFactor = pow(dot(LightReflect,eyeDirection), shininess);
			vSpecularLight = lightcolor * max(SpecularFactor, 0.0);
		}

		fragColor = vec4((vAmbientLight + vDiffuseLight + vSpecularLight)*diffuse,alpha);
        //fragColor = vec4(shininess/255.0);

		//float Far = 1000.0;
		//float Near = 0.1;
		//float A = - (zFar + zNear) / (zFar - zNear);
		//float B = (2.0 * zFar * zNear) / (zFar - zNear);
		//if(tc.x < 0.0)
			//fragColor = vec3( (2.0 * Near) / (Far + Near - depth * (Far - Near)) );//
	}
	else
	{
		fragColor = vec4(diffuse,alpha); // alpha was wrong because alpha tex of plants is rendered always
	}
}