#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;  // Position of the vertices of the full-screen quad
layout(location = 1) in vec2 a_UV;        // UV coordinates for screen-space mapping

layout(location = 0) out vec2 fragUV;  // Pass UV to the fragment shader

void main()
{
    // Pass through the UV coordinates
    fragUV = a_UV;

    // Output the vertex position in clip space
    gl_Position = vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform Data {
    vec4 iResolution;
    vec4 iMouse;
    float iTime;
	int iFrame;
};

void main() {
	vec2 uv = fragUV * 1.0 - 0.5;
	uv.x *= iResolution.x / iResolution.y;

	vec3 col = vec3(uv.x, uv.y, 0.0);
    
    fragColor = vec4(col,1);
}
