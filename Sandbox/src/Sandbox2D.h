#pragma once

#include <Bubble.h>

#include <map>

#include "ParticleSystem.h"

enum TileTypes {
	Blank, 
	Up,
	Right,
	Down,
	Left,
	None
};

static const char* TypeToString(TileTypes type)
{
	switch (type)
	{
	case None: return "None";
	case Blank: return "Blank";
	case Up: return "Up";
	case Right: return "Right";
	case Down: return "Down";
	case Left: return "Left";
	default: return "Unknwown";
	}
}

enum TileRotation {
	_Up,
	_Left,
	_Down,
	_Right
};

// Up, Right, Down, Left
using Rule = std::vector<std::vector<TileTypes>>;
static const std::unordered_map<TileTypes, Rule> Rules = {
    { Blank,   {{Blank, Up}, {Blank, Right}, {Blank, Down}, {Blank, Left}} },
    { Up,      {{Right, Left, Down}, {Left, Up, Down}, {Blank, Down}, {Right, Up, Down}} },
    { Right,   {{Right, Left, Down}, {Left, Up, Down}, {Right, Left, Up}, {Blank, Left}} },
    { Down,    {{Blank, Up}, {Left, Up, Down}, {Right, Left, Up}, {Right, Up, Down}} },
    { Left,    {{Right, Left, Down}, {Blank, Right}, {Right, Left, Up}, {Up, Down, Right}} }
};

static const Rule& GetRule(TileTypes& tileType) {
	auto it = Rules.find(tileType);
	if (it != Rules.end()) {
		return it->second;
	}
	// Return an empty rule if the TileType is not found in the Rules map
	static const Rule emptyRule;
	return emptyRule;
}

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
	TileTypes TileType = None;
	//std::vector<TileTypes> Options = { Blank, Up, Right, Left, Down };
	//std::vector<int> Options = {};
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
		TileType = (TileTypes)option;
		Options = { option };
		Collapsed = true;
	}
};

struct Tile {
	Bubble::Ref<Bubble::Texture2D> Texture;
	std::vector<int> Edges;
	int Rotation;

	std::vector<int> Up, Right, Down, Left;

	Tile Rotate(int rotation = 1)
	{
		int len = Edges.size();
		std::vector<int> NewEdges(len);

		for (int i = 0; i < len; i++)
		{
			NewEdges[i] = Edges[(i - rotation + len) % len];
		}

		Tile newTile = { Texture, NewEdges, Rotation + rotation };
		return newTile;
	}

	void Analyze(std::unordered_map<int, Tile>& tiles)
	{
		for (auto& kv : tiles)
		{
			int key = kv.first;
			Tile& tile = kv.second;

			// Up
			if (tile.Edges[2] == Edges[0])
				Up.push_back(key);

			// Right
			if (tile.Edges[3] == Edges[1])
				Right.push_back(key);

			// Down
			if (tile.Edges[0] == Edges[2])
				Down.push_back(key);

			// Left
			if (tile.Edges[1] == Edges[3])
				Left.push_back(key);
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
	virtual void OnImGuiRender() override;
	void OnEvent(Bubble::Event& e) override;

public:
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
	std::unordered_map<int, Tile> m_Tiles;

	bool m_WaveFunctionCollapse = false;

	int m_TestInt = 0;
};