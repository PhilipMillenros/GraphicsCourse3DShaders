#version 330 core

uniform float u_Time;
uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

out vec2 f_Position;
out vec3 f_Color;
out vec2 f_TexCoord;


void main() 
{
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 0.0, 1.0);
	f_Color = a_Color;
	f_Position = a_Position;
	f_TexCoord = a_TexCoord;
}