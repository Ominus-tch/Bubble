#pragma once

#include <Bubble.h>

#include <map>

#include "ParticleSystem.h"

enum TileTypes {
	None,
	Blank, 
	Up,
	Right,
	Down,
	Left
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

static void CheckValid(std::vector<int>& options, std::vector<TileTypes>& valid)
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

struct Tile {
	bool Collapsed = false;
	TileTypes TileType = None;
	//std::vector<TileTypes> Options = { Blank, Up, Right, Left, Down };
	std::vector<int> Options;

	Tile(int numTiles)
	{
		for (int i = 0; i < numTiles; i++)
		{
			Options.push_back(i);
		}
	}

	void PickOption()
	{
		int pick = Bubble::Random::Int(0, Options.size() - 1);
		TileType = Options[pick];
		Options = { TileType };
		Collapsed = true;
	}
};

struct TileSpecs {
	Bubble::Ref<Bubble::Texture2D> Texture;
	std::vector<int> EdgeIds;
	int Rotation;

	TileSpecs Rotate(int rotation = 1)
	{
		int len = EdgeIds.size();
		std::vector<int> NewEdges(len);

		for (int i = 0; i < len; i++)
		{
			NewEdges[i] = EdgeIds[(i - rotation + len) % len];
		}

		TileSpecs newTile = { Texture, NewEdges, Rotation + rotation };
		return newTile;
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
	Tile* GetLowestEntropyTile();
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
	std::vector<Tile*> m_MapTiles;
	std::unordered_map<TileTypes, TileSpecs> m_TileSpecs;

	bool m_WaveFunctionCollapse = false;

	int m_TestInt = 0;
};