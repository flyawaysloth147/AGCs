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

			//Fetch Serial Data;
			if (m_serial->available()) {
				std::string serialData = m_serial->fetchData();
				if(m_showConsole)
					consoleAddLine(serialData);
			}

			// UI Rendering
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			imguiRender();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			m_window->swapBuffer();
		}
	}

	void Application::init()
	{
		SetConsoleOutputCP(CP_UTF8);

		m_window = std::make_shared<Window>(m_width, m_height, m_name);
		m_serial = new SerialInterface(9600, L"COM3", 50, NOPARITY);

		imguiInit();
	}

	void Application::shutdown()
	{
		delete m_serial;

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

	void Application::imguiRender()
	{
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		ImGui::ShowDemoWindow(&m_showDemoWindow);
		ImPlot::ShowDemoWindow(&m_ImPlotShowDemo);

		if (ImGui::Begin("MPU")) {

			ImFont* font = ImGui::GetFont();
			float oldFontSize = ImGui::GetFont()->FontSize;
			font->FontSize = oldFontSize + 10;

			ImGui::PushFont(font);

			ImGui::Text("Roll: ");
			ImGui::Text("Pitch: ");
			ImGui::Text("Yaw: ");

			font->FontSize = oldFontSize;
			ImGui::PopFont();

			ImGui::End();
		}

		if (ImGui::Begin("Barometric & Pressure")) {

			ImGui::Text("Temperature: ");
			ImGui::Text("Pressure: ");
			ImGui::Text("Altitude: ");

			ImGui::End();
		}

		if (ImGui::Begin("Apogee")) {

			ImGui::Text("Apogee: 100m");

			ImGui::End();
		}

		if (ImGui::Begin("Console", &m_showConsole)) {

			if (ImGui::BeginChild("Scrolling")) {
				for (const auto& line : m_consoleBuffer) {
					ImGui::TextWrapped("%s", line.c_str());
				}

				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
					ImGui::SetScrollHereY(1.0f);
				}

				ImGui::EndChild();
			}

			ImGui::End();
		}
	}

	void Application::imguishutdown()
	{
		ImPlot::DestroyContext();
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

	void Application::consoleAddLine(std::string& line)
	{
		if (m_consoleBuffer.size() >= 200) {
			m_consoleBuffer.erase(m_consoleBuffer.begin());
		}

		m_consoleBuffer.push_back(line);
	}
}