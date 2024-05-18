#pragma once

#include <Bubble.h>

#include <map>

#include "ParticleSystem.h"

static void CheckValid(std::vector<int>& options, std::vector<int>& valid)
{

	for (int i = options.size() - 1; i >= 0; i--)
	{
		int option = options[i];
		// If option is not in valid, remove from options
		if (std::find(valid.begin(), valid.end(), option) == valid.end())
		{
			options.erase(options.begin() + i);
		}
	}
}

struct Cell {
	bool Collapsed = false;
	int TileType = -1;
	std::vector<int> Options = {};

	Cell(int numTiles)
	{
		GenerateOptions(numTiles);
	}

	void GenerateOptions(int numTiles)
	{
		for (int i = 0; i < numTiles; i++)
		{
			Options.push_back(i);
		}
	}

	void PickOption()
	{
		int pick = Bubble::Random::Int(0, Options.size() - 1);
		int option = Options[pick];
		TileType = option;
		Options = { option };
		Collapsed = true;
	}
};

static bool CompareEdge(const std::vector<int>& vec1, const std::vector<int>& vec2) 
{
	std::vector<int> reversedVec2 = vec2;
	std::reverse(reversedVec2.begin(), reversedVec2.end());

	// Compare each element
	for (size_t i = 0; i < vec1.size(); ++i) {
		if (vec1[i] != reversedVec2[i]) {
			// If any element is not equal, vectors are not equal
			return false;
		}
	}

	// All elements are equal, vectors are equal
	return true;
}

struct Tile {
	Bubble::Ref<Bubble::Texture2D> Texture;
	std::vector<std::vector<int>> Edges;
	//std::vector<int> Edges;
	int Rotation;

	std::vector<int> Up, Right, Down, Left;

	Tile Rotate(int rotation = 1)
	{
		int len = Edges.size();
		std::vector < std::vector<int>> NewEdges(len);

		for (int i = 0; i < len; i++)
		{
			NewEdges[i] = Edges[(i - rotation + len) % len];
		}

		return { Texture, NewEdges, Rotation - rotation };
	}

	void Analyze(std::vector<Tile>& tiles)
	{
		for (int i = 0; i < tiles.size(); i++)
		{
			Tile& tile = tiles[i];

			// Up
			if (CompareEdge(tile.Edges[2], Edges[0]))
				Up.push_back(i);

			// Right
			if (CompareEdge(tile.Edges[3], Edges[1]))
				Right.push_back(i);

			// Down
			if (CompareEdge(tile.Edges[0], Edges[2]))
				Down.push_back(i);

			// Left
			if (CompareEdge(tile.Edges[1], Edges[3]))
				Left.push_back(i);
		}

	}
};

class Sandbox2D : public Bubble::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Bubble::Timestep ts) override;
	void OnRender(Bubble::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Bubble::Event& e) override;

private:
	Cell* GetLowestEntropyCell();
	void ResetGrid();
private:
	Bubble::OrthographicCameraController m_CameraController;
	ParticleProps m_Particle;
	ParticleSystem m_ParticleSystem;

	// Temp
	Bubble::Ref<Bubble::VertexArray> m_SquareVA;
	Bubble::Ref<Bubble::Shader> m_FlatColorShader;

	Bubble::Ref<Bubble::Texture2D> m_CheckerboardTexture;
	Bubble::Ref<Bubble::Texture2D> m_SpriteSheet;
	Bubble::Ref<Bubble::SubTexture2D> m_SpriteStairs;
	Bubble::Ref<Bubble::SubTexture2D> m_SpriteBarrel;

	Bubble::Ref<Bubble::SubTexture2D> m_SpriteOrangeTree;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	float m_LastFrameTime = 0.f;
	float m_FPS = 0.f;
	uint32_t m_FrameCount = 0;
	
	float m_LoopTime = 0.f;
	bool m_Done = false;

	uint32_t m_MapWidth, m_MapHeight;
	std::vector<Cell*> m_Grid;
	std::vector<Tile> m_Tiles;

	bool m_WaveFunctionCollapse = false;

	int m_TestInt = 0;
};