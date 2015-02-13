#version 440

layout (location = 0) in vec3 Position;
layout (location = 4) in ivec4 BoneId;
layout (location = 5) in vec4 BoneWeight;

uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mat4 gBoneMatrix[40];

out vec3 wpos;

void main()
{
	// combine final transformation
	ivec4 bid = BoneId;
	mat4 transform  = gBoneMatrix[bid[0]] * BoneWeight[0];
	transform += gBoneMatrix[bid[1]] * BoneWeight[1];
	transform += gBoneMatrix[bid[2]] * BoneWeight[2];
	transform += gBoneMatrix[bid[3]] * BoneWeight[3];

    gl_Position = gWVP * transform * vec4(Position, 1.0);
    wpos = vec3(gWorld * transform * vec4(Position, 1.0));
}
