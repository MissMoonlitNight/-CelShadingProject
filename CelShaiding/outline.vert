#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outline_width; 

void main()
{
    vec3 pos = aPos + aNormal * outline_width;
    
    gl_Position = projection * view * model * vec4(pos, 1.0);
}