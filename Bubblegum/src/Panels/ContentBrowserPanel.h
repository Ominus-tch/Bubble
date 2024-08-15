#pragma once

#include "Bubble/Core/Base.h"
#include "Bubble/Scene/Scene.h"
#include "Bubble/Scene/Entity.h"

namespace Bubble {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel() = default;
		ContentBrowserPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}