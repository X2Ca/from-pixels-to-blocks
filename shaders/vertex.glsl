#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aBary;

//in vec3 a_dist;
out vec3 dist;
out vec4 position;
out vec4 minmax;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 MinMax;



void main()
{
    dist = aBary;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    position = model*vec4(aPos, 1.0);
    minmax = MinMax;
}
