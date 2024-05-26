#pragma once

#include "AGCs/Logging/Log.h"
#include "AGCs/Core/Window/Window.h"

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

	private:
		// Application Information !not updated!
		int m_width = 0;
		int m_height = 0;
		std::string m_name;

		// Application Parameter
		bool m_shouldExit = false;

		// Class Variable
		std::shared_ptr<Window> m_window;
	};
}