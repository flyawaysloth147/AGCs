#pragma once

#include "AGCs/Logging/Log.h"
#include "AGCs/Core/Window/Window.h"
#include "AGCs/Core/Serial.h"

#include <implot.h>
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

		void consoleAddLine(std::string& line);
		void VerticalSeparator(float height);

	private:
		// Application Information !not updated!
		int m_width = 0;
		int m_height = 0;
		std::string m_name;

		// Application Parameter
		bool m_shouldExit = false;
		bool m_firstTimeBoot = true;

		// Class Variable
		std::shared_ptr<Window> m_window;
		SerialInterface* m_serial;

		// Imgui window parameter
	private:
		bool m_showDemoWindow = true;
		bool m_ImPlotShowDemo = true;
		bool m_showConsole = true;

		std::vector<std::string> m_consoleBuffer;
	};
}