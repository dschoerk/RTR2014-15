#version 430 core

in vec3 tePosition;
in vec3 teNormal;
in vec2 teTexcoord;
in vec3 tePatchDistance;
//in vec4 shadow_coord;

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
	WorldPosOut	= tePosition;	
    DiffuseOut 	= texture(diffuse, teTexcoord).xyz;

    if(specularMappingOn == 1)
    {
        SpecOut = texture(specular, teTexcoord).xyz;
    }
    else
    {
        SpecOut = vec3(0);
    }

    if(alphaMappingOn == 1)
    {
        AlphaOut = texture(alpha, teTexcoord).xyz;
        if(AlphaOut.x < 0.1)
            discard;
    }
    else
    {
        AlphaOut = vec3(1.0);
    }
    
	vec3 n = normalize(teNormal);	
	n = normalize(n);
	
	if(normalMappingOn == 1)
	{
        vec3 tn = texture(normal, teTexcoord).xyz;	
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
}