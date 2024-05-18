#pragma once

#include "Bubble/Core/Layer.h"

#include "Bubble/Events/KeyEvent.h"
#include "Bubble/Events/MouseEvent.h"
#include "Bubble/Events/ApplicationEvent.h"

namespace Bubble {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
			
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();

		uint32_t GetActiveWidgetID() const;
	private:
		bool m_BlockEvents = false;
	};

}