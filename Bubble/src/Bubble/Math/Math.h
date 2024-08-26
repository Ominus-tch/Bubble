#pragma once

#include "Bubble/Core/Random.h";
#include <glm/glm.hpp>

constexpr float PI = 3.14159265359f;

namespace Bubble {

	namespace Math {

		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

	}

	static float lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	static float smoothstep(float t)
	{
		return t * t * (3.0f - 2.0f * t);
	}

	static float gradient(int hash, float x)
	{
		// Use the Next function to generate a consistent pseudo-random value based on the hash
		Random::Next(static_cast<float>(hash)); // Seed the RNG
		float grad = Random::Float() * 2.0f - 1.0f; // Random float in range [-1, 1]
		return grad * x;
	}

	static float gradient(int seed, float x, float y) {
		// Seed the RNG with the provided seed
		Random::Next(static_cast<float>(seed));

		// Generate a random gradient vector
		float gradX = Random::Float() * 2.0f - 1.0f; // Random value in range [-1, 1]
		float gradY = Random::Float() * 2.0f - 1.0f; // Random value in range [-1, 1]

		// Return the dot product of the gradient and the vector to the input point
		return gradX * x + gradY * y;
	}

	static float noise(float angle) {
		// Normalize the angle to be between 0 and 1
		float normalizedAngle = angle / (2.0f * PI);

		// Calculate grid points
		int x0 = (int)floor(normalizedAngle);   // Left grid point
		int x1 = x0 + 1;                        // Right grid point

		// Calculate relative position in grid cell
		float x = normalizedAngle - (float)x0;

		// Generate random gradient for each grid point
		float grad0 = gradient(x0, x);
		float grad1 = gradient(x1, x - 1.0f);

		// Interpolate between the two gradients
		float t = smoothstep(x);
		return lerp(grad0, grad1, t);
	}

	static float noise(float x, float y) {
		float normalizedX = x / (2.0f * PI);
		float normalizedY = y / (2.0f * PI);

		// Calculate grid points
		int x0 = (int)floor(normalizedX);   // Left grid point for x
		int x1 = x0 + 1;                    // Right grid point for x
		int y0 = (int)floor(normalizedY);   // Top grid point for y
		int y1 = y0 + 1;                    // Bottom grid point for y

		// Calculate relative positions in the grid cell
		float tx = normalizedX - (float)x0;
		float ty = normalizedY - (float)y0;

		// Generate random gradients for each grid point
		float grad00 = gradient(x0 + y0 * 57, tx, ty);
		float grad01 = gradient(x0 + y1 * 57, tx, ty - 1.0f);
		float grad10 = gradient(x1 + y0 * 57, tx - 1.0f, ty);
		float grad11 = gradient(x1 + y1 * 57, tx - 1.0f, ty - 1.0f);

		// Interpolate between the gradients along x
		float u = smoothstep(tx);
		float v = smoothstep(ty);

		float lerpX0 = lerp(grad00, grad10, u); // Interpolate along the top edge
		float lerpX1 = lerp(grad01, grad11, u); // Interpolate along the bottom edge

		// Interpolate between the top and bottom edges along y
		return lerp(lerpX0, lerpX1, v);
	}

}
