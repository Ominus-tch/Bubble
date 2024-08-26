#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include <unordered_map>
#include <vector>

#include "SwapEndian.h"

struct TableInfo
{
	uint32_t checksum;
	uint32_t offset;
	uint32_t length;
};

struct Point
{
	int x;
	int y;
	bool OnCurve;
};

struct GlyphData
{
	std::vector<Point> Points;
	std::vector<int> contourEndIndices;

	std::string ToString() {
		std::ostringstream oss;

		// Print contourEndIndices
		for (size_t i = 0; i < contourEndIndices.size(); ++i) {
			oss << "End Indice " << i + 1 << ": " << contourEndIndices[i] << "\n";
		}

		// Print points (x, y)
		for (size_t i = 0; i < Points.size(); ++i) {
			oss << "Point " << i + 1 << ": (" << Points[i].x << ", " << Points[i].y << ")\n";
		}

		return oss.str();
	}
};

inline int max(int a, int b) {
	return (a > b) ? a : b;
}


class FontReader
{
public:
	FontReader(const std::string& pathToFont)
	{
		stream = std::ifstream(pathToFont, std::ios::binary);
	}

	std::string ReadTag() {
		char tag[4];
		for (int i = 0; i < 4; i++) {
			tag[i] = stream.get();  // Read a byte and store it as a char
		}

		return std::string(tag, 4);  // Convert the char array to a std::string
	}

	template <typename T>
	T Read()
	{
		T value;
		stream.read(reinterpret_cast<char*>(&value), sizeof(T));

		//stream.seekg(sizeof(T), std::ios::cur);

		value = swapEndian(value);

		return value;
	}

	std::vector<Point> ReadCoordinates(std::vector<char> flags)
	{
		// Flag masks
		const int OnCurve = 0;
		const int IsSingleByteX = 1;
		const int IsSingleByteY = 2;
		const int Repeat = 3;
		const int InstructionX = 4;
		const int InstructionY = 5;

		std::vector<Point> points(flags.size());

		for (int j = 0; j < 2; j++)
		{
			bool readingX = j == 0;

			int singleByteFlagBit = readingX ? IsSingleByteX : IsSingleByteY;
			int instructionFlagMask = readingX ? InstructionX : InstructionY;

			int coordVal = 0;

			for (int i = 0; i < flags.size(); i++)
			{
				char currFlag = flags[i];

				// Offset value is represented with 1 byte (unsigned)
				// Here the instruction flag tells us whether to add or subtract the offset
				if (FlagBitIsSet(currFlag, singleByteFlagBit))
				{
					int coordOffset = Read<char>();
					bool positiveOffset = FlagBitIsSet(currFlag, instructionFlagMask);
					coordVal += positiveOffset ? coordOffset : -coordOffset;
				}
				// Offset value is represented with 2 bytes (signed)
				// Here the instruction flag tells us whether an offset value exists or not
				else if (!FlagBitIsSet(currFlag, instructionFlagMask))
				{
					coordVal += Read<int16_t>();
				}

				if (readingX) points[i].x = coordVal;
				else points[i].y = coordVal;
				points[i].OnCurve = FlagBitIsSet(currFlag, OnCurve);
			}
		}

		return points;
	}

	void Skip(size_t bytes)
	{
		stream.seekg(bytes, std::ios::cur);
	}

	void Goto(size_t offset)
	{
		stream.seekg(offset, std::ios::beg);
	}

	uint64_t GetPos()
	{
		return stream.tellg();
	}

	bool FlagBitIsSet(char flag, int bitIndex)
	{
		return ((flag >> bitIndex) & 1) == 1;
	}

	GlyphData ReadSimpleGlyph()
	{
		std::vector<int> contourEndIndices(Read<int16_t>());
		Skip(8);

		for (int i = 0; i < contourEndIndices.size(); i++)
		{
			contourEndIndices[i] = Read<uint16_t>();
		}

		int numPoints = contourEndIndices.back() + 1;
		BG_INFO("{0} points!", numPoints);

		std::vector<char> allFlags(numPoints);
		int instructionsLength = Read<int16_t>();
		Skip(instructionsLength);

		for (int i = 0; i < numPoints; i++)
		{
			char flag = Read<char>();
			allFlags[i] = flag;

			if (FlagBitIsSet(flag, 3))
			{
				for (int r = 0; r < Read<char>(); r++)
				{
					i++;
					allFlags[i] = flag;
				}
			}
		}

		std::vector<Point> points = ReadCoordinates(allFlags);
		GlyphData data = { points, contourEndIndices };
		return data;
	}

	std::unordered_map<std::string, TableInfo> TableInfo;

private:
	std::ifstream stream;
};