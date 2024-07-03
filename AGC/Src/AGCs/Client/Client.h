#pragma once

#include "AGCs/Core/Application.h"

namespace Client {
	class MainLayer : public AGC::Layer {
	public:
		MainLayer();
		~MainLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(float dt = 1.0f) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		void consoleAddLine(std::string& line);

		void VerticalSeparator(float height);

	private:
		AGC::SerialInterface* m_serial = nullptr;

	private: // ImGui variable
		bool m_showDemoWindow = true;
		bool m_ImPlotShowDemo = true;
		bool m_showConsole = true;
		std::vector<std::string> m_consoleBuffer;
	};

	class App : public AGC::Application {
	public:
		App()
			: Application(700, 800, "Axantera Ground Control system") {
			pushLayer(new MainLayer());
		}
	};
}