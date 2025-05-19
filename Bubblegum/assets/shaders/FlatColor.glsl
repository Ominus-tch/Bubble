// Flat Color Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

void main()
{
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(std140, binding = 1) uniform Uniforms
{
	vec4 u_Color;
};
layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = vec4(0.5, 0.5, 0.4, 1.0);
}
