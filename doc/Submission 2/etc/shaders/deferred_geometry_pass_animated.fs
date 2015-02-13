#version 440

in vec3 Normal0; 
in vec3 WorldPos0; 
in vec2 TexCoord0; 
in vec3 Color0;

layout (location = 0) out vec3 WorldPosOut; 
layout (location = 1) out vec3 DiffuseOut; 
layout (location = 2) out vec3 NormalOut; 
layout (location = 3) out vec3 SpecOut; 
layout (location = 4) out vec3 AlphaOut; 

uniform sampler2D diffuse; 
uniform sampler2D normal; 
uniform sampler2D specular;
uniform sampler2D alpha;

uniform int normalMappingOn;
uniform int specularMappingOn;
uniform int alphaMappingOn;

void main()	
{	
    WorldPosOut	= WorldPos0;	
    DiffuseOut 	= texture(diffuse, TexCoord0).xyz;
	//if(length(DiffuseOut) < 0.01)
	//	DiffuseOut = vec3(1,0,0);

    if(specularMappingOn == 1)
    {
        SpecOut = texture(specular, TexCoord0).xyz;
    }
    else
    {
        SpecOut = vec3(0);
    }

    if(alphaMappingOn == 1)
    {
        AlphaOut = texture(alpha, TexCoord0).xyz;
        if(AlphaOut.x < 0.1)
            discard;
    }
    else
    {
        AlphaOut = vec3(1.0);
    }
    
	vec3 n = normalize(Normal0);	
	n = normalize(n);
	
	if(normalMappingOn == 1)
	{
        vec3 tn = texture(normal, TexCoord0).xyz;	
		vec3 up = vec3(0,1,0);
		vec3 tangent = normalize(cross(up, tn));
		vec3 bitangent = normalize(cross(tn, tangent));
		mat3 tbn = mat3(tangent,bitangent,tn);
		NormalOut = transpose(tbn) * n;
	}
	else
	{
		NormalOut = n;
	}
	NormalOut = n;
	//DiffuseOut = Color0;
}