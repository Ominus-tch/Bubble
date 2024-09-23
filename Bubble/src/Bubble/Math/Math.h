#pragma once

#include "Bubble/Core/Random.h";
#include <glm/glm.hpp>

constexpr float PI = 3.14159265359f;

namespace Bubble {

	namespace Math {

		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);

	}


	// Sha256 Functions
	inline uint32_t rotr(uint32_t x, uint32_t n) {
		return (x >> n) | (x << (32 - n));
	}

	// SHA-256 Functions as defined in the standard
	inline uint32_t Ch(uint32_t x, uint32_t y, uint32_t z) {
		return (x & y) ^ (~x & z);
	}

	inline uint32_t Maj(uint32_t x, uint32_t y, uint32_t z) {
		return (x & y) ^ (x & z) ^ (y & z);
	}

	inline uint32_t e0(uint32_t x) {
		return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
	}

	inline uint32_t e1(uint32_t x) {
		return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
	}

	inline uint32_t a0(uint32_t x) {
		return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
	}

	inline uint32_t a1(uint32_t x) {
		return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
	}

	static const std::array<uint32_t, 64> K = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	static std::array<uint32_t, 8> H = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
	};

	// Padding message and processing it in 512-bit chunks
	static std::vector<uint8_t> padMessage(const std::string& input) {
		std::vector<uint8_t> paddedMessage(input.begin(), input.end());

		// Add a '1' bit followed by '0' bits
		paddedMessage.push_back(0x80);

		// Pad with 0's until the message length is congruent to 448 mod 512
		while ((paddedMessage.size() * 8) % 512 != 448) {
			paddedMessage.push_back(0);
		}

		// Append the original message length in bits, as a 64-bit big-endian integer
		uint64_t bitLength = input.size() * 8;
		for (int i = 7; i >= 0; --i) {
			paddedMessage.push_back(static_cast<uint8_t>(bitLength >> (i * 8)));
		}

		return paddedMessage;
	}

	// Processing each 512-bit chunk of the message
	static void processChunk(const uint8_t chunk[64]) {
		std::array<uint32_t, 64> W = { 0 };

		// Prepare message schedule
		for (int i = 0; i < 16; ++i) {
			W[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) | (chunk[i * 4 + 2] << 8) | chunk[i * 4 + 3];
		}
		for (int i = 16; i < 64; ++i) {
			W[i] = a1(W[i - 2]) + W[i - 7] + a0(W[i - 15]) + W[i - 16];
		}

		// Initialize working variables
		uint32_t a = H[0], b = H[1], c = H[2], d = H[3];
		uint32_t e = H[4], f = H[5], g = H[6], h = H[7];

		// Perform main hash computation
		for (int i = 0; i < 64; ++i) {
			uint32_t T1 = h + e1(e) + Ch(e, f, g) + K[i] + W[i];
			uint32_t T2 = e0(a) + Maj(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;
		}

		// Add the compressed chunk to the current hash value
		H[0] += a;
		H[1] += b;
		H[2] += c;
		H[3] += d;
		H[4] += e;
		H[5] += f;
		H[6] += g;
		H[7] += h;
	}

	static std::string sha256(const std::string& input) {
		// Step 1: Pad the input message
		std::vector<uint8_t> paddedMessage = padMessage(input);

		// Step 2: Process the message in 512-bit chunks
		for (size_t i = 0; i < paddedMessage.size(); i += 64) {
			processChunk(&paddedMessage[i]);
		}

		// Step 3: Produce the final hash value (big-endian)
		std::stringstream hashStream;
		for (uint32_t h : H) {
			hashStream << std::hex << std::setw(8) << std::setfill('0') << h;
		}

		return hashStream.str();
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
