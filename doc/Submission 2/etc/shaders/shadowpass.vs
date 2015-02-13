#version 440 

layout (location = 0) in vec3 Position; 

uniform mat4 gMVP;
uniform mat4 modelMatrix;

out vec3 wpos;
out vec4 vpos;

void main()
{ 
    gl_Position = gMVP * modelMatrix * vec4(Position, 1.0); 	
	wpos = vec3(modelMatrix * vec4(Position, 1.0)); 	
	vpos = gMVP * modelMatrix * vec4(Position, 1.0); 	
}