#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;    // Vertex position in world space
layout(location = 1) in vec4 a_Color;       // Vertex color
layout(location = 2) in int a_EntityID;     // Entity ID for editor purposes

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec4 Color;
};

layout(location = 0) out VertexOutput Output;
layout(location = 1) out flat int v_EntityID;

void main()
{
	Output.Color = a_Color;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
	vec4 Color;
};

layout(location = 0) in VertexOutput Input;
layout(location = 1) in flat int v_EntityID;

void main()
{
	// Output the interpolated color from the vertex shader
	o_Color = Input.Color;

	// Output the entity ID for editor or selection purposes
	o_EntityID = v_EntityID;
}
