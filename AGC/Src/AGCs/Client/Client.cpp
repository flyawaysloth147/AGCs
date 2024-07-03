#include "AGCpch.h"

#include "Client.h"

namespace Client {
	MainLayer::MainLayer() {

	}

	MainLayer::~MainLayer() {
		
	}

	void MainLayer::OnAttach() {
		m_serial = new AGC::SerialInterface(9600, L"COM3", 50, NOPARITY);

	}

	void MainLayer::OnDetach() {
		delete m_serial;
	}

	void MainLayer::OnUpdate(float dt) {
		//Fetch Serial Data;
		if (m_serial->available()) {
			std::string serialData = m_serial->fetchData();
			if (m_showConsole)
				consoleAddLine(serialData);
		}
	}

	void MainLayer::OnRender() {

	}

	void MainLayer::OnImGuiRender() {
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

			float verticalSeparatorHeight = 15.0f;

			ImGui::Text("COM Port: %s", m_serial->getPort().c_str());

			{
				VerticalSeparator(verticalSeparatorHeight);
				ImGui::Text("Baud Rate: %d", m_serial->getBaudRate());
			}

			{
				VerticalSeparator(verticalSeparatorHeight);
				ImGui::Text("Connection Status: ");
				ImGui::SameLine();
				std::string conStatus = (m_serial->opened()) == true ? "Connected" : "Disconnected"; // string for connection status
				if (m_serial->opened())
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // connected = green
				else
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // disconnected = red

				ImGui::Text("%s", conStatus.c_str());
				ImGui::PopStyleColor();
			}

			{
				VerticalSeparator(verticalSeparatorHeight);
				ImGui::Text("Max Queue Size: %d", m_serial->maxQueueSize());
			}

			{
				VerticalSeparator(verticalSeparatorHeight);
				ImGui::Text("Current Queue Size: ");
				ImGui::SameLine();

				unsigned int currentQueueSize = m_serial->currentQueueSize();
				if (currentQueueSize < 10)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				else if (currentQueueSize > 10 && currentQueueSize <= 20)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
				else
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

				ImGui::Text("%d", currentQueueSize);
				ImGui::PopStyleColor();
			}

			{
				ImVec4 childBgColor = { 0.01f, 0.01f, 0.01f, 1.0f }; // console background color

				ImGui::PushStyleColor(ImGuiCol_ChildBg, childBgColor);

				ImGui::BeginChild("Scrolling", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Border);
				for (const auto& line : m_consoleBuffer) {
					ImGui::TextWrapped("%s", line.c_str());
				}

				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
					ImGui::SetScrollHereY(1.0f);
				}

				ImGui::EndChild();


				ImGui::PopStyleColor();
			}

			ImGui::End();
		}

		if (AGC::Application::get()->isFirstTimeBoot()) {
			std::string consoleMessage = "-----Serial Console-----";
			consoleAddLine(consoleMessage);
		}
	}

	void MainLayer::consoleAddLine(std::string& line)
	{
		if (m_consoleBuffer.size() >= 200) {
			m_consoleBuffer.erase(m_consoleBuffer.begin());
		}

		m_consoleBuffer.push_back(line);
	}

	void MainLayer::VerticalSeparator(float height) {
		ImGui::SameLine();
		ImGui::GetWindowDrawList()->AddLine(
			ImGui::GetCursorScreenPos(),
			ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + height),
			ImGui::GetColorU32(ImGuiCol_Border)
		);
		ImGui::Dummy(ImVec2(5.0f, height)); // Add spacing to the right of the separator
		ImGui::SameLine();
	}
}