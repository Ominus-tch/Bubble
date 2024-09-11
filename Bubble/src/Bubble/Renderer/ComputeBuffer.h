#pragma once

#include "Bubble/Core/Base.h"

namespace Bubble {

	class ComputeBuffer
	{
	public:
		virtual ~ComputeBuffer() = default;

		virtual void Bind(uint32_t slot) const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data) = 0;
		virtual void GetData(void* data) = 0;

		virtual void Release() = 0;

		virtual uint32_t GetCount() const = 0;
		virtual uint32_t GetStride() const = 0;

		virtual uint32_t GetRendererID() const = 0;

		static Ref<ComputeBuffer> Create(uint32_t count, uint32_t stride);

	};

}
