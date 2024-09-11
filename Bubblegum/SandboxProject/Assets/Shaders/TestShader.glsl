#type compute
#version 450 core

layout(local_size_x = 1) in;

layout(std430, binding = 0) buffer OutputBuffer
{
	uint data[];
};

void main()
{
	// Get the global invocation ID, which is a unique index for each thread
	uint index = gl_GlobalInvocationID.x;

	// Store the index in the output buffer
	data[index] = index;
}
