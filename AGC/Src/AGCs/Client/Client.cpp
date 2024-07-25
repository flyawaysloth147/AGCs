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

		ImGuiIO& io = ImGui::GetIO();
		{
			ImFontConfig fontConfig;
			fontConfig.SizePixels = 18.0f;
			fontConfig.OversampleH = 3;
			fontConfig.OversampleV - 1;
			ImFont* font = nullptr;

			font = io.Fonts->AddFontFromFileTTF("Font/Roboto-Light.ttf", 18.0f, &fontConfig);
			io.FontDefault = font;
		}
		{
			ImFontConfig fontConfig;
			fontConfig.SizePixels = 18.0f;
			fontConfig.OversampleH = 3;
			fontConfig.OversampleV - 1;
			ImFont* font = nullptr;

			font = io.Fonts->AddFontFromFileTTF("Font/Roboto-Light.ttf", 25.0f, &fontConfig);
		}

		m_imGuiFontMap["LargeFont"] = io.Fonts->Fonts[1];
	}

	void MainLayer::OnDetach() {
		delete m_serial;
	}

	void MainLayer::OnUpdate(float dt) {
		m_deltaTime = dt;
		//Fetch Serial Data;
		if (m_serial->available()) {
			std::string serialData = m_serial->fetchData();

			std::pair<std::string, glm::vec4> consoleOut = parseData(serialData);
			if (m_imGuiShowConsole) {
				if (m_consoleShowRaw)
					consoleAddLine(serialData);
				else
					consoleAddLine(consoleOut.first, consoleOut.second);
				
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
			graphVar.roll.AddPoint(t, m_flightData.rotation.x);
			graphVar.pitch.AddPoint(t, m_flightData.rotation.y);
			graphVar.yaw.AddPoint(t, m_flightData.rotation.z);
		}
	}

	void MainLayer::OnRender() {

	}

	void MainLayer::OnImGuiRender() {
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		ImGui::ShowDemoWindow(&m_showDemoWindow);
		ImPlot::ShowDemoWindow(&m_ImPlotShowDemo);

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Window")) {
				if (ImGui::MenuItem("Console Output")) {
					m_imGuiShowConsole = !m_imGuiShowConsole;
				}
				ImGui::SameLine();
				ImGui::Checkbox("##ConsoleCheckbox", &m_imGuiShowConsole);
				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();

		if (ImGui::Begin("MPU")) {
			GraphVariable::RotationGraph& graphVar = m_graphVariable.rotationGraph;
			static float t = 0.0f;
			t += m_deltaTime;

			if (ImPlot::BeginPlot("Rotation Graph", ImVec2(-1, 0), ImPlotFlags_Crosshairs)) {

				constexpr ImPlotAxisFlags XaxisFlags = ImPlotAxisFlags_NoTickLabels;

				ImPlot::SetupAxes(nullptr, nullptr, XaxisFlags, 0);
				ImPlot::SetupAxisLimits(ImAxis_X1, t - 10.0f, t, ImGuiCond_Always);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 0);

				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImPlot::PlotLine("Roll", &graphVar.roll.Data[0].x, &graphVar.roll.Data[0].y, graphVar.roll.Data.size(), 0, graphVar.roll.Offset, sizeof(glm::vec2));
				ImPlot::PopStyleColor();

				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImPlot::PlotLine("Pitch", &graphVar.pitch.Data[0].x, &graphVar.pitch.Data[0].y, graphVar.pitch.Data.size(), 0, graphVar.pitch.Offset, sizeof(glm::vec2));
				ImPlot::PopStyleColor();

				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				ImPlot::PlotLine("Yaw", &graphVar.yaw.Data[0].x, &graphVar.yaw.Data[0].y, graphVar.yaw.Data.size(), 0, graphVar.yaw.Offset, sizeof(glm::vec2));
				ImPlot::PopStyleColor();

				ImPlot::EndPlot();
			}
			{
				float windowSize = ImGui::GetWindowSize().x;
				std::string text1 = std::format("Roll: %.2f", m_flightData.rotation.x);;
				std::string text2 = std::format("Pitch: %.2f", m_flightData.rotation.y);
				std::string text3 = std::format("Yaw: %.2f", m_flightData.rotation.z);
				float text1Size = ImGui::CalcTextSize(text1.c_str()).x;
				float text2Size = ImGui::CalcTextSize(text2.c_str()).x;
				float text3Size = ImGui::CalcTextSize(text3.c_str()).x;
				float totalWidth = text1Size + text2Size + text3Size;
				float cursorPos = (windowSize - totalWidth) / 4.0f;
				ImGui::SetCursorPosX(cursorPos);

				ImFont* font = m_imGuiFontMap["LargeFont"];
				static float seperatorHeight = 5.0f;

				ImGui::PushFont(font);
				addTextWithBackground(glm::vec4(1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), "Roll: %.2f", m_flightData.rotation.x);
				ImGui::SameLine();
				VerticalSeparator(seperatorHeight);

				addTextWithBackground(glm::vec4(1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.5f), "Pitch: %.2f", m_flightData.rotation.y);
				ImGui::SameLine();
				VerticalSeparator(seperatorHeight);

				addTextWithBackground(glm::vec4(1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.5f), "Yaw: %.2f", m_flightData.rotation.z);
				ImGui::SameLine();
				VerticalSeparator(seperatorHeight);
				ImGui::PopFont();
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

		if (m_imGuiShowConsole) {
			ImGui::Begin("Console", &m_imGuiShowConsole);

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
				ImGui::Text("Max Queue Size: %d", m_serial->maxQueueSize()); // TODO: Need some fix
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
					ImVec4 textColor{ line.second.r, line.second.g, line.second.b, line.second.a };
					ImGui::PushStyleColor(ImGuiCol_Text, textColor);
					ImGui::TextWrapped("%s", line.first.c_str());
					ImGui::PopStyleColor();
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

				ImGui::Separator();

				ImGui::Checkbox("Raw Output", &m_consoleShowRaw);

				ImGui::End();
				
			}
		}

	}

	void MainLayer::consoleAddLine(std::string& line, glm::vec4 textColor)
	{
		if (line.empty())
			return;

		if (m_consoleBuffer.size() >= 200) {
			m_consoleBuffer.erase(m_consoleBuffer.begin());
		}

		m_consoleBuffer.push_back(std::make_pair(line, textColor));
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

	void MainLayer::addTextWithBackground(const glm::vec4& textColor, const glm::vec4& backgroundColor, const char* text, ...) {
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 textPos = ImGui::GetCursorScreenPos();

		// Get the size of the formatted text
		char buffer[256]; // A buffer for the formatted text
		va_list args;
		va_start(args, text);
		vsnprintf(buffer, sizeof(buffer), text, args);

		ImVec2 textSize = ImGui::CalcTextSize(buffer);

		ImVec2 rectMin = textPos;
		ImVec2 rectMax = ImVec2(textPos.x + textSize.x, textPos.y + textSize.y);

		ImU32 bgColor32 = ImGui::ColorConvertFloat4ToU32({ backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a });

		drawList->AddRectFilled(rectMin, rectMax, bgColor32);

		ImGui::SetCursorScreenPos(textPos); // Reset cursor position after drawing the rectangle
		ImGui::PushStyleColor(ImGuiCol_Text, { textColor.r, textColor.g, textColor.b, textColor.a });
		ImGui::TextV(text, args);
		va_end(args);
		ImGui::PopStyleColor();
	}

	std::pair<std::string, glm::vec4> MainLayer::parseData(std::string& str) {
		int dataType = 0;
		std::string out;
		glm::vec4 outColor{ 1.0f };

		if (str.empty())
			return std::make_pair("", outColor);

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
			out = "Parachute is deployed";
			break;
		}

		default:
		{
			out = std::format("Unexpected value parameter: {}", str.c_str());
			outColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		}
		}

		return std::make_pair(out, outColor);
	}
}