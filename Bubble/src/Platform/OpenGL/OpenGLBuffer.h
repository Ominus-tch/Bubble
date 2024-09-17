#pragma once

#include "Bubble/Renderer/Buffer.h"

namespace Bubble {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void UnBind() const;

		virtual uint32_t GetCount() const { return m_Count; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

	class OpenGLComputeBuffer : public ComputeBuffer
	{
	public:
		OpenGLComputeBuffer(uint32_t count, uint32_t stride);
		virtual ~OpenGLComputeBuffer();

		virtual void Bind(uint32_t slot) const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data) override;
		virtual void GetData(void* data) override;

		virtual void Release() override;

		virtual uint32_t GetCount() const override { return m_Count; }
		virtual uint32_t GetStride() const override { return m_Stride; }

		virtual uint32_t GetRendererID() const override;

	private:
		uint32_t m_RendererID;

		uint32_t m_Count;
		uint32_t m_Stride;

		uint32_t GetBufferSize() const { return m_Count * m_Stride; }
	};

}
