#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
{
}

void Sandbox2D::OnAttach()
{
	BG_PROFILE_FUNCTION()

	m_CameraController.ToggleRotation();
	m_CameraController.SetZoomLevel(27);

	m_CheckerboardTexture = Bubble::Texture2D::Create("assets/textures/Checkerboard.png");
	m_SpriteSheet = Bubble::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

	//s_TextureMap['D'] = Bubble::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6, 11 }, { 128, 128 });
	//s_TextureMap['W'] = Bubble::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 });

	//m_SpriteBarrel = Bubble::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 8, 2 }, { 128, 128 });

	Bubble::Ref<Bubble::Texture2D> BlankTexture = Bubble::Texture2D::Create("assets/game/textures/blank.png");
	//s_TextureMap[TileTypes::Blank] = BlankTexture;

	Bubble::Ref<Bubble::Texture2D> UpTexture = Bubble::Texture2D::Create("assets/game/textures/up.png");
	//s_TextureMap[TileTypes::Up] = UpTexture;
	//s_TextureMap[TileTypes::Right] = UpTexture;
	//s_TextureMap[TileTypes::Down] = UpTexture;
	//s_TextureMap[TileTypes::Left] = UpTexture;

	//s_TextureMap[TileTypes::Right] = Bubble::Texture2D::Create("assets/game/textures/right.png");
	//s_TextureMap[TileTypes::Down] = Bubble::Texture2D::Create("assets/game/textures/down.png");
	//s_TextureMap[TileTypes::Left] = Bubble::Texture2D::Create("assets/game/textures/left.png");
	 
	Tile upTileSpecs =	{ UpTexture, {1, 1, 0, 1} };
	Tile leftTileSpecs	= upTileSpecs.Rotate(1);
	Tile downTileSpecs	= upTileSpecs.Rotate(2);
	Tile rightTileSpecs = upTileSpecs.Rotate(3);

	m_Tiles[TileTypes::Blank]	= { BlankTexture, {0, 0, 0, 0} };
	m_Tiles[TileTypes::Up]		= upTileSpecs;
	m_Tiles[TileTypes::Right]	= rightTileSpecs;
	m_Tiles[TileTypes::Down]	= downTileSpecs;
	m_Tiles[TileTypes::Left]	= leftTileSpecs;

	for (auto& kv : m_Tiles)
	{
		Tile& tileSpecs = kv.second;
		tileSpecs.Analyze(m_Tiles);
	}

	m_MapWidth = 50;
	m_MapHeight = 50;

	ResetGrid();

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 2.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
}

void Sandbox2D::OnDetach()
{
	BG_PROFILE_FUNCTION()
}

void Sandbox2D::OnUpdate(Bubble::Timestep ts)
{
	BG_PROFILE_FUNCTION()

	m_LoopTime += ts;

	m_FPS = 1.f / ts;

	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Bubble::Renderer2D::ResetStats();
	{
		BG_PROFILE_SCOPE("Renderer Prep")
		Bubble::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Bubble::RenderCommand::Clear();
	}

#if 0
	{
		BG_PROFILE_SCOPE("Renderer Draw")

		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		Bubble::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Bubble::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		Bubble::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Bubble::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
		Bubble::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
		Bubble::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, glm::radians(rotation), m_CheckerboardTexture, 20.0f);
		Bubble::Renderer2D::EndScene();

		Bubble::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Bubble::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		Bubble::Renderer2D::EndScene();
	}

	if (Bubble::Input::IsMouseButtonPressed(BG_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Bubble::Input::GetMousePos();
		auto width = Bubble::Application::Get().GetWindow().GetWidth();
		auto height = Bubble::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
		{
			m_Particle.turningLeft = !m_Particle.turningLeft;
			m_ParticleSystem.Emit(m_Particle);
		}
	}
#endif

	{
		BG_PROFILE_SCOPE("Renderer WaveFunctionCollapse");

		if (m_WaveFunctionCollapse && !m_Done)
		{
			//if (m_LoopTime >= 0.01f)
			{
				BG_PROFILE_SCOPE("Update WaveFunctionCollapse");
				//m_LoopTime = 0.f;

				Cell* lowestEntropyTile = GetLowestEntropyCell();
				if (lowestEntropyTile)
				{
					lowestEntropyTile->PickOption();
				}

				std::vector<Cell*> nextTiles;
				nextTiles.reserve(m_Grid.size());

				int size = m_Tiles.size();

				for (uint32_t y = 0; y < m_MapHeight; y++)
				{
					for (uint32_t x = 0; x < m_MapWidth; x++)
					{
						int i = x + y * m_MapWidth;
						Cell* currentTile = m_Grid[i];

						if (currentTile->Collapsed)
						{
							nextTiles.push_back(currentTile);
							continue;
						}

						Cell* newTile = new Cell(size);
						std::vector<int> validOptions;

						// Look up
						if (y < m_MapHeight - 1)
						{
							Cell* up = m_Grid[x + (y + 1) * m_MapWidth];
							for (int option : up->Options)
							{
								auto& valid = m_Tiles[option].Down;
								validOptions.insert(validOptions.end(), valid.begin(), valid.end());
							}
							CheckValid(newTile->Options, validOptions);
						}

						// Look right
						if (x < m_MapWidth - 1)
						{
							Cell* right = m_Grid[x + 1 + y * m_MapWidth];
							validOptions.clear();
							for (int option : right->Options)
							{
								auto& valid = m_Tiles[option].Left;
								validOptions.insert(validOptions.end(), valid.begin(), valid.end());
							}
							CheckValid(newTile->Options, validOptions);
						}


						// Look down
						if (y > 0)
						{
							Cell* down = m_Grid[x + (y - 1) * m_MapWidth];
							validOptions.clear();
							for (int option : down->Options)
							{
								auto& valid = m_Tiles[option].Up;
								validOptions.insert(validOptions.end(), valid.begin(), valid.end());
							}
							CheckValid(newTile->Options, validOptions);
						}

						// Look left
						if (x > 0)
						{
							Cell* left = m_Grid[x - 1 + y * m_MapWidth];
							validOptions.clear();
							for (int option : left->Options)
							{
								auto& valid = m_Tiles[option].Right;
								validOptions.insert(validOptions.end(), valid.begin(), valid.end());
							}
							CheckValid(newTile->Options, validOptions);
						}

						//CheckValid(newTile->Options, validOptions);

						if (newTile->Options.size() == 0)
							newTile->GenerateOptions(size);

						nextTiles.push_back(newTile);
					}
				}

				m_Grid = std::move(nextTiles);
			}
		}

		Bubble::Renderer2D::BeginScene(m_CameraController.GetCamera());

		float width = m_MapWidth, height = m_MapHeight;
		float halfWidth = width * 0.5f, halfHeight = height * 0.5f;

		Bubble::Renderer2D::DrawRect({ -0.5f, -0.5f, -0.1f }, { width, height }, glm::vec4(1));

		for (uint32_t x = 0; x < m_MapWidth - 1; x++)
		{
			Bubble::Renderer2D::DrawLine({ x - halfWidth + 0.5f, halfHeight - 0.5f, -0.1f }, { x - halfWidth + 0.5f, -halfHeight - 0.5f, -0.1f }, glm::vec4(1));
		}
 
		for (uint32_t y = 0; y < m_MapHeight; y++)
		{
			if (y != m_MapHeight - 1)
			{
				Bubble::Renderer2D::DrawLine({ -halfWidth - 0.5f, y - halfHeight + 0.5f, -0.1f }, { halfWidth - 0.5f, y - halfHeight + 0.5f, -0.1f }, glm::vec4(1));
			}

			for (uint32_t x = 0; x < m_MapWidth; x++)
			{
				int idx = x + y * m_MapWidth;
				Cell* tile = m_Grid[idx];

				if (tile->TileType == None)
					continue;

				Tile tileSpecs = m_Tiles[tile->TileType];
				float rotation = tileSpecs.Rotation * 90.f;

				//BG_INFO("Rotation: {0} -> {1}", tileSpecs.Rotation, rotation);

				Bubble::Renderer2D::DrawRotatedQuad({ x - m_MapWidth / 2.f, y - m_MapHeight / 2.f }, { 1.0f, 1.0f }, glm::radians(rotation), tileSpecs.Texture);
			}
		}

		//Bubble::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.5f }, { 1.0f, 1.0f }, m_SpriteStairs);
		//Bubble::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.5f }, { 1.0f, 1.0f }, m_SpriteBarrel);
		//Bubble::Renderer2D::DrawQuad({ -1.0f, 0.0f, 0.5f }, { 1.0f, 2.0f }, m_SpriteOrangeTree);

		Bubble::Renderer2D::EndScene();
	}

	m_FrameCount++;

	//m_ParticleSystem.OnUpdate(ts);
	//m_ParticleSystem.OnRender(m_CameraController.GetCamera());
}

void Sandbox2D::OnImGuiRender()
{
	BG_PROFILE_FUNCTION()

	ImGui::Begin("Settings");

	//auto stats = Bubble::Renderer2D::GetStats();
	//ImGui::Text("Renderer2D Stats:");
	//ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	//ImGui::Text("Quads: %d", stats.QuadCount);
	//ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	//ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::Text("FPS: %f", m_FPS);

	ImGui::Checkbox("Wave Functon Collapse", &m_WaveFunctionCollapse);
	if (ImGui::Button("Reset"))
		ResetGrid();

	ImGui::InputInt("Test", &m_TestInt);

	ImGui::End();
}

void Sandbox2D::OnEvent(Bubble::Event& e)
{
	m_CameraController.OnEvent(e);
}

bool sortByOptionsSize(const Cell* cell1, const Cell* cell2) {
	return cell1->Options.size() < cell2->Options.size();
}

void Sandbox2D::ResetGrid()
{
	m_Grid.clear();

	int size = m_Tiles.size();

	for (uint32_t y = 0; y < m_MapHeight; y++)
	{
		for (uint32_t x = 0; x < m_MapWidth; x++)
		{
			m_Grid.push_back(new Cell(size));
		}
	}

	m_Done = false;
}

Cell* Sandbox2D::GetLowestEntropyCell()
{
	BG_PROFILE_FUNCTION()

	std::vector<Cell*> sortedCells(m_Grid);

	for (auto it = sortedCells.begin(); it != sortedCells.end();) {
		if ((*it)->Collapsed) {
			it = sortedCells.erase(it);
		}
		else {
			++it;
		}
	}

	if (sortedCells.size() == 0)
	{
		m_Done = true;
		return nullptr;
	}

	std::sort(sortedCells.begin(), sortedCells.end(), sortByOptionsSize);

	int len = sortedCells[0]->Options.size();
	bool collpased = sortedCells[0]->Collapsed;

	int stopIdx = 0;
	for (int i = 1; i < sortedCells.size(); i++)
	{
		if (sortedCells[i]->Options.size() > len)
		{
			stopIdx = i;
			break;
		}
	}
	if (stopIdx == 0)
	{
		int pick = Bubble::Random::Int(0, sortedCells.size() - 1);
		return sortedCells[pick];
	}

	std::vector<Cell*> lowestEntropyCells(sortedCells.begin(), sortedCells.begin() + stopIdx);
	int pick = Bubble::Random::Int(0, lowestEntropyCells.size() - 1);

	return lowestEntropyCells[pick];
}
