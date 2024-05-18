#pragma once

#include "Bubble/Core/Window.h"
#include "Bubble/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Bubble
{

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline uint32_t GetWidth() const override { return m_Data.Width; };
		inline uint32_t GetHeight() const override { return m_Data.Height; };

		// Window attribtes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; };
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const { return m_Window; };
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		} m_Data;
	};

}