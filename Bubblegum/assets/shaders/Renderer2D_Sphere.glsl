#type vertex
#version 450 core

layout(location = 0) in vec3 a_WorldPosition;  // 3D world position
layout(location = 1) in vec3 a_LocalPosition;  // Local position within the sphere
layout(location = 2) in vec4 a_Color;          // Color
layout(location = 3) in float a_Thickness;     // Thickness of the border
layout(location = 4) in float a_Fade;          // Fade effect
layout(location = 5) in int a_EntityID;        // Entity ID

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout(location = 0) out VertexOutput Output;
layout(location = 4) out flat int v_EntityID;

void main()
{
	Output.LocalPosition = a_LocalPosition;   // Local space (inside the sphere)
	Output.Color = a_Color;
	Output.Thickness = a_Thickness;
	Output.Fade = a_Fade;

	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_WorldPosition, 1.0);  // Transform to clip space
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;   // Output color
layout(location = 1) out int o_EntityID; // Output Entity ID

struct VertexOutput
{
	vec3 LocalPosition;   // 3D local position inside the sphere
	vec4 Color;           // Color
	float Thickness;      // Thickness of the border
	float Fade;           // Fade effect
};

layout(location = 0) in VertexOutput Input;   // Input from the vertex shader
layout(location = 4) in flat int v_EntityID;  // Entity ID from the vertex shader

void main()
{
	// Calculate the distance from the center of the sphere in local coordinates
	vec3 localPos = Input.LocalPosition;

	// Normalize the local position to get the normal of the sphere at this fragment
	vec3 normal = normalize(localPos);

	// Simple lighting model: diffuse lighting with a single light source
	vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));  // Light coming from (1, 1, 1) direction
	float diffuse = max(dot(normal, lightDir), 0.0);  // Basic diffuse lighting

	// Set the color based on the lighting and the input color
	vec4 color = Input.Color;
	color.rgb *= diffuse;  // Apply diffuse lighting to the RGB channels

	// Output the final color and entity ID
	o_Color = color;
	o_EntityID = v_EntityID;
}
