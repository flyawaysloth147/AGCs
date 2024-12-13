#include "AGCpch.h"

#include "Application.h"

namespace AGC {

	Application* Application::m_instance;

	Application::Application(int width, int height, const char* name)
	{
		m_width = width;
		m_height = height;
		m_name = name;

		m_instance = this;

		init();
	}

	Application::~Application()
	{
		shutdown();
	}

	void Application::run()
	{
		for (Layer* layer : m_layerStack) {
			layer->OnAttach();
		}

		while (!m_shouldExit && !m_window->shouldClose()) {
			m_window->pollEvent();
			m_window->clear();

			for (Layer* layer : m_layerStack) {
				layer->OnUpdate(ImGui::GetIO().DeltaTime); //temp
			}

			for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
				(*(--it))->OnRender();
			}

			// UI Rendering
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
				(*(--it))->OnImGuiRender();
			}

			// Render toasts on top of everything, at the end of your code!
			// You should push style vars here
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f); // Round borders
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f, 100.f / 255.f)); // Background color
			ImGui::RenderNotifications(); // <-- Here we render all notifications
			ImGui::PopStyleVar(1); // Don't forget to Pop()
			ImGui::PopStyleColor(1);

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


			m_window->swapBuffer();
			m_firstTimeBoot = false;
		}

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*(--it))->OnDetach();
		}
	}

	void Application::init()
	{
		SetConsoleOutputCP(CP_UTF8);
		m_window = std::make_shared<Window>(m_width, m_height, m_name);

		imguiInit();
	}

	void Application::shutdown()
	{
		imguishutdown();
	}

	void Application::imguiInit()
	{
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(m_window->getWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 430");
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	void Application::imguishutdown()
	{
		ImPlot::DestroyContext();
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}
}