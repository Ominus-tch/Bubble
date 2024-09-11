#pragma once

#include "Bubble/Renderer/ComputeBuffer.h"

namespace Bubble {

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
