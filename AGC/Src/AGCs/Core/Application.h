#pragma once

#include "AGCs/Logging/Log.h"
#include "AGCs/Core/Window/Window.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>


#include <string>

namespace AGC {
	class Application {
	public:
		Application(int width, int height, const char* name);
		~Application();

		void run();

	private:
		// Helper Function
		void init();
		void shutdown();

		void imguiInit();
		void imguiRender();
		void imguishutdown();

	private:
		// Application Information !not updated!
		int m_width = 0;
		int m_height = 0;
		std::string m_name;

		// Application Parameter
		bool m_shouldExit = false;

		// Class Variable
		std::shared_ptr<Window> m_window;

		// Imgui window parameter
	private:
		bool m_showDemoWindow = true;
	};
}