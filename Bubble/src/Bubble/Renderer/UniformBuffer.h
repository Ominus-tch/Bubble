#pragma once

#include "Bubble/Core/Base.h"

namespace Bubble {

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {}
		virtual void SetData(const void* data, uint32_t offset = 0) = 0;

		virtual uint32_t GetSize() = 0;
		virtual uint32_t GetBinding() = 0;

		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};

}
