#include "AGCpch.h"

#include "Client.h"

namespace Client {
	MainLayer::MainLayer() {

	}

	MainLayer::~MainLayer() {
		
	}

	void MainLayer::OnAttach() {
		m_serial = new AGC::SerialInterface(9600, L"COM3", 50, NOPARITY);
		m_consoleBuffer.reserve(200);
	}

	void MainLayer::OnDetach() {
		delete m_serial;
	}

	void MainLayer::OnUpdate(float dt) {
		m_deltaTime = dt;
		//Fetch Serial Data;
		if (m_serial->available()) {
			std::string serialData = m_serial->fetchData();
			std::string consoleOut = parseData(serialData);
			if (m_showConsole) {
				consoleAddLine(consoleOut);
			}
		}

		if (AGC::Application::get()->isFirstTimeBoot()) {
			std::string consoleMessage = "-----Serial Console-----";
			consoleAddLine(consoleMessage);
		}

		{
			GraphVariable::RotationGraph& graphVar = m_graphVariable.rotationGraph; //Rotation graph variable
			static float t = 0;
			t += dt;

			graphVar.rotation.AddPoint(t, m_flightData.rotation.x, m_flightData.rotation.y, m_flightData.rotation.z);
		}
	}

	void MainLayer::OnRender() {

	}

	void MainLayer::OnImGuiRender() {
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		ImGui::ShowDemoWindow(&m_showDemoWindow);
		ImPlot::ShowDemoWindow(&m_ImPlotShowDemo);

		if (ImGui::Begin("Global Settings")) {
			ImGui::SliderFloat("Graph sample lenght", &m_graphVariable.SampleLenght, 1, 30, "%.1f s");

			ImGui::End();
		}

		if (ImGui::Begin("MPU")) {
			GraphVariable::RotationGraph& graphVar = m_graphVariable.rotationGraph;
			static ImPlotAxisFlags flags = ImPlotAxisFlags_AutoFit;   

			static float t = 0.0f;
			t += m_deltaTime;

			if (ImPlot::BeginPlot("##Rolling", ImVec2(-1, 150))) {

				constexpr ImPlotAxisFlags axisFlags = ImPlotAxisFlags_AutoFit;

				ImPlot::SetupAxes(nullptr, nullptr, 0, axisFlags);
				ImPlot::SetupAxisLimits(ImAxis_X1, t - 10.0f, t, ImGuiCond_Always);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 0);
				ImPlot::PlotLine("Roll", &graphVar.rotation.Data[0].x, &graphVar.rotation.Data[0].y, graphVar.rotation.Data.size(), 0, graphVar.rotation.Offset, sizeof(glm::vec2));
				ImPlot::PlotLine("Pitch", &graphVar.rotation.Data[0].x, &graphVar.rotation.Data[0].z, graphVar.rotation.Data.size(), flags, graphVar.rotation.Offset, sizeof(glm::vec4));
				ImPlot::PlotLine("Yaw", &graphVar.rotation.Data[0].x, &graphVar.rotation.Data[0].w, graphVar.rotation.Data.size(), flags, graphVar.rotation.Offset, sizeof(glm::vec4));
				ImPlot::EndPlot();
			}
			{
				static float seperatorHeight = 5.0f;

				ImGui::Text("Roll: %f", m_flightData.rotation.x);
				ImGui::SameLine();
				VerticalSeparator(seperatorHeight);

				ImGui::Text("Pitch: %f", m_flightData.rotation.y);
				ImGui::SameLine();
				VerticalSeparator(seperatorHeight);

				ImGui::Text("Yaw: %f", m_flightData.rotation.z);
				ImGui::SameLine();
				VerticalSeparator(seperatorHeight);
			}

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

		if (m_showConsole) {
			ImGui::Begin("Console", &m_showConsole);

			constexpr float verticalSeparatorHeight = 15.0f;

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

		{
			if (ImGui::Begin("Settings")) {
				{
						ImGui::BeginDisabled(m_reconnectButtonDisable);

					if (ImGui::Button("Reconnect")) {
						std::wstringstream ss;
						ss << L"COM" << m_port;
						m_serial->recreateConnection(m_baudRate, ss.str(), 50, NOPARITY);
						m_reconnectButtonDisable = true;
					}

						ImGui::EndDisabled();
				}

				ImGui::Separator();

				{ // if the value is change it will enable the reconnect button
					if (ImGui::InputInt("Port", &m_port))
						m_reconnectButtonDisable = false;
					if (ImGui::InputInt("Baudrate", &m_baudRate))
						m_reconnectButtonDisable = false;
				}

				ImGui::End();
			}
		}

	}

	void MainLayer::consoleAddLine(std::string& line)
	{
		if (line.empty())
			return;

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

	std::string MainLayer::parseData(std::string& str) {
		if (str.empty())
			return "";

		int dataType = 0;
		std::string out;
		sscanf(str.c_str(), "%d", &dataType);

		switch (dataType)
		{
		case FlightDataTypeRotation:
		{
			sscanf(str.c_str(), "%*d %f %f %f", &m_flightData.rotation.x, &m_flightData.rotation.y, &m_flightData.rotation.z);
			out = std::format("Rotation: Roll {} Pitch {} Yaw {}", m_flightData.rotation.x, m_flightData.rotation.y, m_flightData.rotation.z);
			break;
		}

		case FlightDataTypePressure:
		{
			sscanf(str.c_str(), "%*d %f", &m_flightData.presure);
			out = std::format("Pressure: {}", m_flightData.presure);
			break;
		}

		case FlightDataTypeTemprature:
		{
			sscanf(str.c_str(), "%*d %f", &m_flightData.temprature);
			out = std::format("Temprature: {}", m_flightData.temprature);
			break;
		}

		case FlightDataTypeAltitude: 
		{
			sscanf(str.c_str(), "%*d %f", &m_flightData.altitude);
			out = std::format("Altitude: {}", m_flightData.altitude);
			break;
		}

		case FlightDataTypeHighestAltitude:
		{
			sscanf(str.c_str(), "%*d %f", &m_flightData.apogee);
			out = std::format("Apogee: {}", m_flightData.apogee);
			break;
		}

		case FlightDataTypePayloadSeperated:
		{
			m_flightData.isPayloadSeparated = true;
			out = "Payload is separated";
			break;
		}

		case FlightDataTypeParachuteDeployed:
		{
			m_flightData.isParachuteDeployed = true;
			out = "Parachute is deploted";
			break;
		}

		default:
		{
			out = std::format("Unexpected value parameter: {}", str.c_str());
			break;
		}
		}

		return out;
	}
}