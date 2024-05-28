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
		ImGui_ImplGlfw_InitForOpenGL(m_window->getWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 430");
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	void Application::imguiRender()
	{
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// DockSpace
		//ImGuiIO& io = ImGui::GetIO();
		//if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		//{
		//	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		//	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		//
		//	static auto first_time = true;
		//	if (first_time)
		//	{
		//		first_time = false;
		//
		//		ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
		//		ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
		//		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
		//
		//		auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
		//		ImGui::DockBuilderDockWindow("Control Panel", dock_id_right);
		//		ImGui::DockBuilderDockWindow("Relight window", dockspace_id);
		//		ImGui::DockBuilderFinish(dockspace_id);
		//	}
		//}

		ImGui::End();

		ImGui::ShowDemoWindow(&m_showDemoWindow);

		ImGui::Begin("Global Docking Window");

		ImGui::Button("press me");

		ImGui::End();

	}

	void Application::imguishutdown()
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

}