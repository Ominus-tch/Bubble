#type vertex
#version 450

layout(std140, binding = 0) uniform Camera
{
	vec3 ViewParams;
	mat4 u_CameraToWorld;
};

layout(std140, binding = 2) uniform Settings {
    ivec2 dims;
    int numShapes;
    vec3 u_Light;
    int u_PositionLight;
};

//layout(std430, binding = 3) buffer ShapesBuffer {
//    Shape shapes[];
//};
//
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
    ivec2 id = ivec2(gl_GlobalInvocationID.xy);
    uint index = id.y * dims.x + id.x;

    vec3 color = vec3(1.0, 0.1, 0.1);

    data[index] = vec4(color, 1.0);
}

#type frag
