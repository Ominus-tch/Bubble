#include "bgpch.h"

#include "Bubble/Renderer/Renderer2D.h"
#include "Bubble/Renderer/UniformBuffer.h"
#include "Bubble/Renderer/VertexArray.h"
#include "Bubble/Renderer/Shader.h"
#include "Bubble/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace Bubble {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		// Editor-only
		int EntityID;
	};

	struct TriVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor-only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor-only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor-only
		int EntityID;
	};

	//struct TextVertex
	//{
	//	glm::vec3 Position;
	//	glm::vec4 Color;
	//	glm::vec2 TexCoord;

	//	// TODO: bg color for outline/bg

	//	// Editor-only
	//	int EntityID;
	//};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxQuadVertices = MaxQuads * 4;
		static const uint32_t MaxQuadIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		static const uint32_t MaxTris = 20000;
		static const uint32_t MaxTriVertices = MaxTris * 3;
		static const uint32_t MaxTriIndices = MaxTris * 3;

		static const uint32_t MaxCircles = 20000;
		static const uint32_t MaxCircleVertices = MaxCircles * 4;
		static const uint32_t MaxCircleIndices = MaxCircles * 6;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		Ref<VertexArray> TriVertexArray;
		Ref<VertexBuffer> TriVertexBuffer;
		Ref<Shader> TriShader;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		Ref<VertexArray> TextVertexArray;
		Ref<VertexBuffer> TextVertexBuffer;
		Ref<Shader> TextShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t TriIndexCount = 0;
		TriVertex* TriVertexBufferBase = nullptr;
		TriVertex* TriVertexBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		//uint32_t TextIndexCount = 0;
		//TextVertex* TextVertexBufferBase = nullptr;
		//TextVertex* TextVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		BG_PROFILE_FUNCTION()

		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxQuadVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Float,  "a_TexIndex"     },
			{ ShaderDataType::Float,  "a_TilingFactor" },
			{ ShaderDataType::Int,    "a_EntityID"     }
			});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxQuadVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxQuadIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxQuadIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxQuadIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Triangles
		s_Data.TriVertexArray = VertexArray::Create();

		s_Data.TriVertexBuffer = VertexBuffer::Create(s_Data.MaxTriVertices * sizeof(TriVertex));
		s_Data.TriVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.TriVertexArray->AddVertexBuffer(s_Data.TriVertexBuffer);

		s_Data.TriVertexBufferBase = new TriVertex[s_Data.MaxTriVertices];

		uint32_t* triangleIndices = new uint32_t[s_Data.MaxTriIndices];

		offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxTriIndices; i += 3)
		{
			triangleIndices[i + 0] = offset + 0;
			triangleIndices[i + 1] = offset + 1;
			triangleIndices[i + 2] = offset + 2;

			offset += 3;
		}

		Ref<IndexBuffer> triangleIB = IndexBuffer::Create(triangleIndices, s_Data.MaxTriIndices);
		s_Data.TriVertexArray->SetIndexBuffer(triangleIB);
		delete[] triangleIndices;

		// Circles
		s_Data.CircleVertexArray = VertexArray::Create();

		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxQuadVertices * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade"          },
			{ ShaderDataType::Int,    "a_EntityID"      }
			});
		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB); // Use quad IB
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxQuadVertices];

		// Lines
		s_Data.LineVertexArray = VertexArray::Create();

		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxQuadVertices * sizeof(LineVertex));
		s_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
			});
		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxQuadVertices];

		// White Texture
		s_Data.WhiteTexture = Texture2D::Create(TextureSpecification());
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");
		s_Data.TriShader = Shader::Create("assets/shaders/Renderer2D_Triangle.glsl");
		s_Data.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");
		s_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");
		//s_Data.TextShader = Shader::Create("assets/shaders/Renderer2D_Text.glsl");

		// Set first texture slot to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		BG_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;
		delete[] s_Data.TriVertexBufferBase;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		BG_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		BG_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		BG_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		BG_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::StartBatch()
	{
		BG_PROFILE_FUNCTION();

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TriIndexCount = 0;
		s_Data.TriVertexBufferPtr = s_Data.TriVertexBufferBase;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineVertexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		//s_Data.TextIndexCount = 0;
		//s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		BG_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			s_Data.QuadShader->Bind();
			RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.TriIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.TriVertexBufferPtr - (uint8_t*)s_Data.TriVertexBufferBase);
			s_Data.TriVertexBuffer->SetData(s_Data.TriVertexBufferBase, dataSize);

			s_Data.TriShader->Bind();
			RenderCommand::DrawIndexed(s_Data.TriVertexArray, s_Data.TriIndexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

			s_Data.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

			s_Data.LineShader->Bind();
			RenderCommand::SetLineWidth(s_Data.LineWidth);
			RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
			s_Data.Stats.DrawCalls++;
		}

		/*if (s_Data.TextIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.TextVertexBufferPtr - (uint8_t*)s_Data.TextVertexBufferBase);
			s_Data.TextVertexBuffer->SetData(s_Data.TextVertexBufferBase, dataSize);

			auto buf = s_Data.TextVertexBufferBase;
			s_Data.FontAtlasTexture->Bind(0);

			s_Data.TextShader->Bind();
			RenderCommand::DrawIndexed(s_Data.TextVertexArray, s_Data.TextIndexCount);
			s_Data.Stats.DrawCalls++;
		}*/
	}

	void Renderer2D::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
	{
		// Half dimensions for easy calculations
		glm::vec3 halfSize = size * 0.5f;

		glm::vec4 redColor = color;
		glm::vec4 greenColor = color;
		glm::vec4 blueColor = color;

		//glm::vec4 redColor = glm::vec4(1.0f, 0.0f, 0.0f, color.a);   // Red for X-axis
		//glm::vec4 greenColor = glm::vec4(0.0f, 1.0f, 0.0f, color.a); // Green for Y-axis
		//glm::vec4 blueColor = glm::vec4(0.0f, 0.0f, 1.0f, color.a);  // Blue for Z-axis

		// Front face (z+)
		DrawQuad(
			position + glm::vec3(0.0f, 0.0f, halfSize.z),
			glm::vec2(size.x, size.y),
			glm::vec3(0.0f, 0.0f, 0.0f),  // No rotation needed, facing forward
			blueColor
		);

		// Back face (z-) - Rotate 180 degrees around Y-axis
		DrawQuad(
			position + glm::vec3(0.0f, 0.0f, -halfSize.z),
			glm::vec2(size.x, size.y),
			glm::vec3(0.0f, 180.0f, 0.0f),  // Rotated to face backward
			blueColor
		);

		// Left face (x-) - Rotate 90 degrees around Y-axis
		DrawQuad(
			position + glm::vec3(-halfSize.x, 0.0f, 0.0f),
			glm::vec2(size.z, size.y),
			glm::vec3(0.0f, 90.0f, 0.0f),  // Rotated to face left
			redColor
		);

		// Right face (x+) - Rotate -90 degrees around Y-axis
		DrawQuad(
			position + glm::vec3(halfSize.x, 0.0f, 0.0f),
			glm::vec2(size.z, size.y),
			glm::vec3(0.0f, -90.0f, 0.0f),  // Rotated to face right
			redColor
		);

		// Top face (y+) - Rotate -90 degrees around X-axis
		DrawQuad(
			position + glm::vec3(0.0f, halfSize.y, 0.0f),
			glm::vec2(size.x, size.z),
			glm::vec3(-90.0f, 0.0f, 0.0f),  // Rotated to face up
			greenColor
		);

		// Bottom face (y-) - Rotate 90 degrees around X-axis
		DrawQuad(
			position + glm::vec3(0.0f, -halfSize.y, 0.0f),
			glm::vec2(size.x, size.z),
			glm::vec3(90.0f, 0.0f, 0.0f),  // Rotated to face down
			greenColor
		);
	}

	void Renderer2D::DrawCubeOutlines(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
	{
		// Half dimensions for easy calculations
		glm::vec3 halfSize = size * 0.5f;

		// Define the 8 vertices of the cube
		glm::vec3 vertices[8] = {
			position + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z),  // 0: Bottom-left-back
			position + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z),   // 1: Bottom-right-back
			position + glm::vec3(halfSize.x, halfSize.y, -halfSize.z),    // 2: Top-right-back
			position + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z),   // 3: Top-left-back
			position + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z),   // 4: Bottom-left-front
			position + glm::vec3(halfSize.x, -halfSize.y, halfSize.z),    // 5: Bottom-right-front
			position + glm::vec3(halfSize.x, halfSize.y, halfSize.z),     // 6: Top-right-front
			position + glm::vec3(-halfSize.x, halfSize.y, halfSize.z)     // 7: Top-left-front
		};

		// Draw the 12 edges of the cube
		DrawLine(vertices[0], vertices[1], color);  // Bottom back edge
		DrawLine(vertices[1], vertices[2], color);  // Right back edge
		DrawLine(vertices[2], vertices[3], color);  // Top back edge
		DrawLine(vertices[3], vertices[0], color);  // Left back edge

		DrawLine(vertices[4], vertices[5], color);  // Bottom front edge
		DrawLine(vertices[5], vertices[6], color);  // Right front edge
		DrawLine(vertices[6], vertices[7], color);  // Top front edge
		DrawLine(vertices[7], vertices[4], color);  // Left front edge

		DrawLine(vertices[0], vertices[4], color);  // Bottom left edge (connecting front and back)
		DrawLine(vertices[1], vertices[5], color);  // Bottom right edge (connecting front and back)
		DrawLine(vertices[2], vertices[6], color);  // Top right edge (connecting front and back)
		DrawLine(vertices[3], vertices[7], color);  // Top left edge (connecting front and back)
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec3& rotation, const glm::vec4& color)
	{
		BG_PROFILE_FUNCTION();

		// Create a transformation matrix for the position
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

		// Apply rotations around the X, Y, and Z axes
		transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // Rotation around X-axis
		transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotation around Y-axis
		transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Rotation around Z-axis

		// Apply scaling for the size of the quad
		transform = glm::scale(transform, glm::vec3(size.x, size.y, 1.0f));

		// Now draw the quad using the transformation matrix
		DrawQuad(transform, color);
	}

	void Renderer2D::NextBatch()
	{
		BG_PROFILE_FUNCTION();

		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		BG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}
	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		BG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		BG_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = {  { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		const float tilingFactor = 1.0f;

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxQuadIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };


		if (s_Data.QuadIndexCount >= Renderer2DData::MaxQuadIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		constexpr size_t quadVertexCount = 4;
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxQuadIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = -1;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		BG_PROFILE_FUNCTION();

		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		BG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		BG_PROFILE_FUNCTION();

		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}
	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		BG_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, subtexture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<SubTexture2D>& subtexture, float tilingFactor, const glm::vec4& tintColor)
	{
		BG_PROFILE_FUNCTION()

		constexpr size_t quadVertexCount = 4;
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		const Ref<Texture2D> texture = subtexture->GetTexture();

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxQuadIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = -1;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	/*void Renderer2D::DrawTriangleFilled(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec4 color, int entityID)
	{
	}*/

	void Renderer2D::DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec4& color, int entityID)
	{
		constexpr size_t triangleVertexCount = 3;

		if (s_Data.TriIndexCount >= Renderer2DData::MaxTriIndices)
			NextBatch();

		// Transform is identity since we're working with direct vertex positions
		glm::mat4 transform = glm::mat4(1.0f);

		// Define the three vertices of the triangle

		s_Data.TriVertexBufferPtr->Position = transform * glm::vec4(a, 1.0f);
		s_Data.TriVertexBufferPtr->Color = color;
		//s_Data.TriVertexBufferPtr->EntityID = -1;
		s_Data.TriVertexBufferPtr++;

		s_Data.TriVertexBufferPtr->Position = transform * glm::vec4(b, 1.0f);
		s_Data.TriVertexBufferPtr->Color = color;
		//s_Data.TriVertexBufferPtr->EntityID = -1;
		s_Data.TriVertexBufferPtr++;

		s_Data.TriVertexBufferPtr->Position = transform * glm::vec4(c, 1.0f);
		s_Data.TriVertexBufferPtr->Color = color;
		//s_Data.TriVertexBufferPtr->EntityID = -1;
		s_Data.TriVertexBufferPtr++;

		s_Data.TriIndexCount += 3;

		s_Data.Stats.TriCount++;
	}

	void Renderer2D::DrawCircle(const glm::vec3 pos, const float r, const glm::vec4 color)
	{
		int segments = 100;
		float angleStep = 2 * PI / segments;
		glm::vec3 previousPoint = pos + glm::vec3(r, 0, 0);

		for (int i = 1; i <= segments; ++i) {
			float angle = i * angleStep;
			glm::vec3 newPoint = pos + glm::vec3(r * cos(angle), r * sin(angle), 0.f);
			DrawLine(previousPoint, newPoint, color);
			previousPoint = newPoint;
		}
	}

	void Renderer2D::DrawCircleFilled(const glm::vec3 pos, const float r, const glm::vec4 color)
	{
		auto transform = glm::translate(glm::mat4(1.0f), pos) *
			glm::scale(glm::mat4(1.0f), glm::vec3(r * 2.f, r * 2.f, 1.0f));
		
		DrawCircleFilled(glm::mat4(transform), color);
	}

	void Renderer2D::DrawCircleFilled(const glm::mat4& transform, const glm::vec4& color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/, int entityID /*= -1*/)
	{
		BG_PROFILE_FUNCTION();

		// TODO: implement for circles
		 if (s_Data.CircleIndexCount >= Renderer2DData::MaxCircleIndices)
		 	NextBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr->EntityID = entityID;
			s_Data.CircleVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawArc(const glm::vec3& pos, float r, float startAngle, float endAngle, const glm::vec4& color) {
		int segments = 100;
		float angleStep = (endAngle - startAngle) / segments;
		glm::vec3 previousPoint = pos + glm::vec3(r * cos(startAngle), r * sin(startAngle), 0);

		for (int i = 1; i <= segments; ++i) {
			float angle = startAngle + i * angleStep;
			glm::vec3 newPoint = pos + glm::vec3(r * cos(angle), r * sin(angle), 0);
			DrawLine(previousPoint, newPoint, color);
			previousPoint = newPoint;
		}
	}

	void Renderer2D::DrawSemiCircle(const glm::vec3& position, float radius, float angle, float startAngle, float endAngle, const glm::vec4& color, int segments)
	{
		glm::vec3 center = position; // Center of the semi-circle

		// Convert angles from degrees to radians
		startAngle = glm::radians(startAngle);
		endAngle = glm::radians(endAngle);
		angle = glm::radians(angle);

		// Calculate the total rotation to apply
		float totalStartAngle = startAngle + angle;
		float totalEndAngle = endAngle + angle;

		// Calculate the angle increment based on the number of segments
		float angleIncrement = (totalEndAngle - totalStartAngle) / segments;

		// Iterate over the segments to draw the triangles
		for (int i = 0; i < segments; ++i)
		{
			float theta1 = totalStartAngle + angle + i * angleIncrement;
			float theta2 = totalStartAngle + angle + (i + 1) * angleIncrement;

			glm::vec3 p1 = center + glm::vec3(radius * cos(theta1), radius * sin(theta1), 0.0f);
			glm::vec3 p2 = center + glm::vec3(radius * cos(theta2), radius * sin(theta2), 0.0f);

			// Draw the triangle for this segment
			DrawTriangle(center, p1, p2, color);
		}
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		BG_PROFILE_FUNCTION();

		if (s_Data.LineVertexCount >= Renderer2DData::MaxQuadVertices)
			NextBatch();

		s_Data.LineVertexBufferPtr->Position = p0;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = p1;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->EntityID = entityID;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexCount += 2;

		s_Data.Stats.LineCount++;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		BG_PROFILE_FUNCTION();

		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color, entityID);
		DrawLine(p1, p2, color, entityID);
		DrawLine(p2, p3, color, entityID);
		DrawLine(p3, p0, color, entityID);
	}
	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		BG_PROFILE_FUNCTION();

		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_Data.QuadVertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color, entityID);
		DrawLine(lineVertices[1], lineVertices[2], color, entityID);
		DrawLine(lineVertices[2], lineVertices[3], color, entityID);
		DrawLine(lineVertices[3], lineVertices[0], color, entityID);
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawQuad(transform, src.Texture, src.TilingFactor, src.Color, entityID);
		else
			DrawQuad(transform, src.Color, entityID);
	}

	void Renderer2D::DrawMesh(const glm::mat4& entityTransform, const MeshComponent& meshComponent, int entityID)
	{
		constexpr size_t triangleVertexCount = 3;

		if (meshComponent.DrawWireframe)
		{
			DrawMeshWireframe(entityTransform, meshComponent, entityID);
		}

		if (!meshComponent.DrawMesh)
			return;

		for (int i = 0; i < meshComponent.Meshes.size(); i++)
		{
			Ref<Mesh> mesh = meshComponent.Meshes[i];

			const glm::mat4& transform = entityTransform * mesh->GetGlobalTransform();
			const auto& Vertices = mesh->GetVertices();
			const auto& Indices = mesh->GetIndices();

			// Check if we need to start a new batch (if we're exceeding the maximum indices or vertices)
			if (s_Data.TriIndexCount + Indices.size() >= Renderer2DData::MaxTriIndices)
				NextBatch();

			for (size_t i = 0; i < Indices.size(); i += triangleVertexCount)
			{

				// Get the indices of the triangle vertices
				uint32_t index0 = Indices[i];
				uint32_t index1 = Indices[i + 1];
				uint32_t index2 = Indices[i + 2];

				// Get the actual vertex positions from the mesh's vertex array
				Vertex v0 = Vertices[index0];
				Vertex v1 = Vertices[index1];
				Vertex v2 = Vertices[index2];

				// Apply the transform to each vertex position
				s_Data.TriVertexBufferPtr->Position = transform * glm::vec4(v0.Position, 1.0f);
				s_Data.TriVertexBufferPtr->Color = v0.Color; // Default color, or you could pass a color as a parameter
				s_Data.TriVertexBufferPtr->EntityID = entityID;
				s_Data.TriVertexBufferPtr++;

				s_Data.TriVertexBufferPtr->Position = transform * glm::vec4(v1.Position, 1.0f);
				s_Data.TriVertexBufferPtr->Color = v1.Color; // Default color
				s_Data.TriVertexBufferPtr->EntityID = entityID;
				s_Data.TriVertexBufferPtr++;

				s_Data.TriVertexBufferPtr->Position = transform * glm::vec4(v2.Position, 1.0f);
				s_Data.TriVertexBufferPtr->Color = v2.Color; // Default color
				s_Data.TriVertexBufferPtr->EntityID = entityID;
				s_Data.TriVertexBufferPtr++;

				// Increment the index count by 3 (for each triangle)
				s_Data.TriIndexCount += triangleVertexCount;

				// Update statistics (optional)
				s_Data.Stats.TriCount++;

				if (s_Data.TriIndexCount + 3 >= Renderer2DData::MaxTriIndices)
					NextBatch();
			}
		}
	}

	void Renderer2D::DrawMeshWireframe(const glm::mat4& entityTransform, const MeshComponent& meshComponent, int entityID)
	{
		constexpr size_t lineVertexCount = 2;

		for (int i = 0; i < meshComponent.Meshes.size(); i++)
		{
			Ref<Mesh> mesh = meshComponent.Meshes[i];

			const glm::mat4& transform = entityTransform * mesh->GetGlobalTransform();
			const auto& Vertices = mesh->GetVertices();
			const auto& Indices = mesh->GetIndices();

			// Check if we need to start a new batch (if we're exceeding the maximum indices or vertices)
			if (s_Data.LineVertexCount + Vertices.size() >= Renderer2DData::MaxQuadVertices)
				NextBatch();

			for (size_t i = 0; i < Indices.size(); i += 3) {
				uint32_t index0 = Indices[i];
				uint32_t index1 = Indices[i + 1];
				uint32_t index2 = Indices[i + 2];

				// Get the positions of the vertices of the triangle
				glm::vec3 p0 = transform * glm::vec4(Vertices[index0].Position, 1.0f);
				glm::vec3 p1 = transform * glm::vec4(Vertices[index1].Position, 1.0f);
				glm::vec3 p2 = transform * glm::vec4(Vertices[index2].Position, 1.0f);

				// Draw the edges of the triangle as lines
				// Edge 1: p0 -> p1
				s_Data.LineVertexBufferPtr->Position = p0;
				s_Data.LineVertexBufferPtr->Color = glm::vec4(0.f, 0.f, 0.f, 1.f); // Default to white color, can be changed as needed
				s_Data.LineVertexBufferPtr->EntityID = entityID;
				s_Data.LineVertexBufferPtr++;

				s_Data.LineVertexBufferPtr->Position = p1;
				s_Data.LineVertexBufferPtr->Color = glm::vec4(0.f, 0.f, 0.f, 1.f); // Default to white color, can be changed as needed
				s_Data.LineVertexBufferPtr->EntityID = entityID;
				s_Data.LineVertexBufferPtr++;

				// Edge 2: p1 -> p2
				s_Data.LineVertexBufferPtr->Position = p1;
				s_Data.LineVertexBufferPtr->Color = glm::vec4(0.f, 0.f, 0.f, 1.f);
				s_Data.LineVertexBufferPtr->EntityID = entityID;
				s_Data.LineVertexBufferPtr++;

				s_Data.LineVertexBufferPtr->Position = p2;
				s_Data.LineVertexBufferPtr->Color = glm::vec4(0.f, 0.f, 0.f, 1.f);
				s_Data.LineVertexBufferPtr->EntityID = entityID;
				s_Data.LineVertexBufferPtr++;

				// Edge 3: p2 -> p0
				s_Data.LineVertexBufferPtr->Position = p2;
				s_Data.LineVertexBufferPtr->Color = glm::vec4(0.f, 0.f, 0.f, 1.f);
				s_Data.LineVertexBufferPtr->EntityID = entityID;
				s_Data.LineVertexBufferPtr++;

				s_Data.LineVertexBufferPtr->Position = p0;
				s_Data.LineVertexBufferPtr->Color = glm::vec4(0.f, 0.f, 0.f, 1.f);
				s_Data.LineVertexBufferPtr->EntityID = entityID;
				s_Data.LineVertexBufferPtr++;

				s_Data.LineVertexCount += lineVertexCount * 3;
				s_Data.Stats.LineCount += 3;

				// Check if we need to start a new batch (if we're exceeding the maximum indices or vertices)
				if (s_Data.LineVertexCount + 3 >= Renderer2DData::MaxQuadVertices)
					NextBatch();
			}
		}
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Data.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Data.LineWidth = width;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
}
