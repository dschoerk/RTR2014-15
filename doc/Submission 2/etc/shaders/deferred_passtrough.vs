#version 430 

layout (location = 0) in vec2 Position; 

void main()
{ 
    gl_Position = vec4(Position,0,1);
}