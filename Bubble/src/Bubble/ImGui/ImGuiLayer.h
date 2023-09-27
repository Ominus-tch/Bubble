#pragma once

#include "Bubble/Layer.h"

#include "Bubble/Events/KeyEvent.h"
#include "Bubble/Events/MouseEvent.h"
#include "Bubble/Events/ApplicationEvent.h"

namespace Bubble {

	class BUBBLE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
			
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.f;
	};

}