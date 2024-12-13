#pragma once

#include "AGCs/Logging/Log.h"
#include "AGCs/Core/Window/Window.h"
#include "AGCs/Core/Serial.h"
#include "AGCs/Core/LayerStack.h"
#include "AGCs/Vendor/ImGui_Notify.h"

#include <implot.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <string>

namespace AGC {
	class Application {
	public:
		Application(int width, int height, const char* name);
		virtual ~Application();

		void run();

		static Application* get() { return m_instance; }
		inline bool isFirstTimeBoot() { return m_firstTimeBoot; }

		void pushLayer(Layer* layer) { m_layerStack.PushLayer(layer); }
		void popLayer(Layer* layer) { m_layerStack.PopLayer(layer); }
		void pushOverlay(Layer* layer) { m_layerStack.PushOverlay(layer); }
		void popOverlay(Layer* layer) { m_layerStack.PopOverlay(layer); }

	private:
		// Helper Function
		void init();
		void shutdown();

		void imguiInit();
		void imguishutdown();

	private:
		// Application Information !not updated!
		int m_width = 0;
		int m_height = 0;
		std::string m_name;

		// Application Parameter
		bool m_shouldExit = false;
		bool m_firstTimeBoot = true;
		static Application* m_instance;

		// Class Variable
		std::shared_ptr<Window> m_window;
		LayerStack m_layerStack;
	};
}