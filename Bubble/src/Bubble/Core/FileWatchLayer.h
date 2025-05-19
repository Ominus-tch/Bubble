#pragma once

#include "Bubble/Core/Layer.h"
#include "Bubble/Core/FileSystem.h"

namespace Bubble {

	class FileWatchLayer : public Layer
	{
	public:
		FileWatchLayer();
		~FileWatchLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;

	private:
		float m_TimeSinceLastScan = 0.f;
		float m_ScanInverval = 1.f;
	};

}
