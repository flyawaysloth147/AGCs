#include "AGCpch.h"

#include "Application.h"

namespace AGC {
	
	Application::Application(int width, int height, const char* name)
	{
		m_width = width;
		m_height = height;
		m_name = name;

		init();
	}

	Application::~Application()
	{
		shutdown();
	}

	void Application::run()
	{
		while (!m_shouldExit && !m_window->shouldClose()) {
			m_window->pollEvent();
			m_window->clear();


			
			m_window->swapBuffer();
		}
	}

	void Application::init()
	{
		m_window = std::make_shared<Window>(m_width, m_height, m_name);
	}

	void Application::shutdown()
	{

	}
}