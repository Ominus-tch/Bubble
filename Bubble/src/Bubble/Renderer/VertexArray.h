#pragma once

#include <memory>
#include "Buffer.h"

namespace Bubble {

	class VertexArray
	{
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) = 0;

		static VertexArray* Create();
	};

}