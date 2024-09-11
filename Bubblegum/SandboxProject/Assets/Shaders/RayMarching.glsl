#type compute
#version 450

layout(local_size_x = 16, local_size_y = 16) in;

// Output: Write-only 2D image
layout(binding = 1, rgba32f) uniform image2D outputTexture;

void main() {
    // Calculate the pixel coordinates we're working on
    ivec2 texCoords = ivec2(gl_GlobalInvocationID.xy);

    vec4 white = vec4(1.0, 1.0, 1.0, 1.0);

    // Store the white color in the output texture at the current pixel
    imageStore(outputTexture, texCoords, white);
}
