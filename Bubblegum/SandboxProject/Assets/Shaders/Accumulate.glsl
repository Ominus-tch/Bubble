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
// Uniforms
uniform sampler2D _MainTex;
uniform sampler2D _PrevFrame;
uniform int _Frame;

layout(location = 0) in vec2 fragUV;                // UV coordinates from the vertex shader
layout(location = 0) out vec4 fragColor;            // Final fragment color output

// Vertex data structure
struct appdata {
    vec4 vertex;
    vec2 uv;
};

// Vertex shader
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;

out vec2 vUV;

void main()
{
    vUV = inUV;
    gl_Position = vec4(inPosition, 1.0);
}

// Fragment shader
void main()
{
    vec4 col = texture(_MainTex, vUV);
    vec4 colPrev = texture(_PrevFrame, vUV);

    float weight = 1.0 / (float(_Frame) + 1.0);
    // Combine previous frame with the current frame.
    vec4 accumulatedCol = clamp(colPrev * (1.0 - weight) + col * weight, 0.0, 1.0);

    FragColor = accumulatedCol;
}
