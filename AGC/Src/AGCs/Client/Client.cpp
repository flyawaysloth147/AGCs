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
			fontConfig.OversampleV = 1;
			ImFont* font = nullptr;

			font = io.Fonts->AddFontFromFileTTF("Font/Roboto-Light.ttf", 18.0f, &fontConfig);
			ImGui::MergeIconsWithLatestFont(16.0f, false);
			io.FontDefault = font;

		}
		{
			ImFontConfig fontConfig;
			fontConfig.SizePixels = 18.0f;
			fontConfig.OversampleH = 3;
			fontConfig.OversampleV = 1;
			ImFont* font = nullptr;

			font = io.Fonts->AddFontFromFileTTF("Font/Roboto-Light.ttf", 25.0f, &fontConfig);
			ImGui::MergeIconsWithLatestFont(16.0f, false);

		}
		{
			ImFontConfig fontConfig;
			fontConfig.SizePixels = 18.0f;
			fontConfig.OversampleH = 3;
			fontConfig.OversampleV = 1;
			ImFont* font = nullptr;

			font = io.Fonts->AddFontFromFileTTF("Font/Roboto-Light.ttf", 30.0f, &fontConfig);
			ImGui::MergeIconsWithLatestFont(16.0f, false);
		}

		m_imGuiFontMap["LargeFont"] = io.Fonts->Fonts[1];
		m_imGuiFontMap["LargeFont-1"] = io.Fonts->Fonts[2];

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

		static float t = 0;
		t += dt;

		{
			GraphVariable::RotationGraph& graphVar = m_graphVariable.rotationGraph; //Rotation graph variable
			graphVar.rotation.AddPoint(t, m_flightData.rotation.x, m_flightData.rotation.y, m_flightData.rotation.z);
		}
		{
			auto& graphVar = m_graphVariable.bmpGraph;
			graphVar.bmp.AddPoint(t, m_flightData.preshure, m_flightData.temprature, m_flightData.altitude);
		} 
		{
			auto& graphVar = m_graphVariable.apogeeGraph;
			graphVar.apogee.AddPoint(t, m_flightData.apogee);
		}
	}

	void MainLayer::OnRender() {

	}

	void MainLayer::OnImGuiRender() {
		static float t = 0.0f;
		t += m_deltaTime;

		{
			// Set the window flags for the menu bar window
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

			ImGuiViewport* viewport = ImGui::GetMainViewport();

			// Set the next window position and size to match the viewport's top area
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, ImGui::GetFrameHeight()));
			ImGui::SetNextWindowViewport(viewport->ID);

			// Create the menu bar window
			ImGui::Begin("Main Menu Bar", nullptr, window_flags);

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("Window")) {
					if (ImGui::MenuItem("Console Output")) {
						m_imGuiShowConsole = !m_imGuiShowConsole;
					}
					ImGui::SameLine();
					ImGui::Checkbox("##ConsoleCheckbox", &m_imGuiShowConsole);
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			ImGui::End();
		}

		{
			// Get the main viewport
			ImGuiViewport* viewport = ImGui::GetMainViewport();

			// Set the next window position and size to match the viewport's area below the menu bar
			ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + ImGui::GetFrameHeight()));
			ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - ImGui::GetFrameHeight()));
			ImGui::SetNextWindowViewport(viewport->ID);

			// Set the window flags for the docking space
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
				ImGuiWindowFlags_NoBackground;

			// Create the window
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::Begin("Main DockSpace", nullptr, window_flags);
			ImGui::PopStyleVar(2);

			// Create the docking space
			ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

			ImGui::End();
		}// Main Docking Window 

		ImGui::ShowDemoWindow(&m_showDemoWindow);
		ImPlot::ShowDemoWindow(&m_ImPlotShowDemo);

		if (ImGui::Begin("MPU", nullptr, ImGuiWindowFlags_MenuBar)) {
			static bool showGraphSize = false;
			static float graphSize = 300.0f;

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("Settings")) {
					if (ImGui::MenuItem("Show Graph Size")) {
						showGraphSize = !showGraphSize;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			if (showGraphSize)
				ImGui::SliderFloat("Graph size", &graphSize, 300.0f, 500.0f);

			ImGui::Separator();

			GraphVariable::RotationGraph& graphVar = m_graphVariable.rotationGraph;

			if (ImPlot::BeginPlot("Rotation Graph", {-1.0f, graphSize}, ImPlotFlags_Crosshairs)) {

				constexpr ImPlotAxisFlags XaxisFlags = ImPlotAxisFlags_NoTickLabels;

				ImPlot::SetupAxes(nullptr, nullptr, XaxisFlags, 0);
				ImPlot::SetupAxisLimits(ImAxis_X1, t - 10.0f, t, ImGuiCond_Always);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1);

				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImPlot::PlotLine("Roll", &graphVar.rotation.Data[0].x, &graphVar.rotation.Data[0].y, graphVar.rotation.Data.size(), 0, graphVar.rotation.Offset, sizeof(glm::vec4));
				ImPlot::PopStyleColor();

				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImPlot::PlotLine("Pitch", &graphVar.rotation.Data[0].x, &graphVar.rotation.Data[0].z, graphVar.rotation.Data.size(), 0, graphVar.rotation.Offset, sizeof(glm::vec4));
				ImPlot::PopStyleColor();

				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				ImPlot::PlotLine("Yaw", &graphVar.rotation.Data[0].x, &graphVar.rotation.Data[0].w, graphVar.rotation.Data.size(), 0, graphVar.rotation.Offset, sizeof(glm::vec4));
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

		if (ImGui::Begin("GPS Location")) {
			static bool isItemClicked = false;
			static bool isItemHovered = false;
			ImFont* font = m_imGuiFontMap["LargeFont"];
			ImFont* font1 = m_imGuiFontMap["LargeFont-1"];

			if (isItemHovered)
				ImGui::PushFont(font1);
			else
				ImGui::PushFont(font);

			isItemHovered = false;

			glm::vec2 windowSize = { ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };
			std::string formatedString = std::format("GPS: %.2f, %.2f", m_flightData.gpsLocation.latitude, m_flightData.gpsLocation.longitude);
			float textSize = ImGui::CalcTextSize(formatedString.c_str()).x;

			glm::vec2 cursorPos = windowSize / 2.0f;
			cursorPos.x -= textSize / 2;
			ImGui::SetCursorPos({ cursorPos.x, cursorPos.y });

			ImGui::Text(formatedString.c_str());

			if (ImGui::IsItemHovered())
				isItemHovered = true;

			if (ImGui::IsItemClicked()) {
				std::string copiedString = "https://www.google.com/maps/place/" + std::to_string(m_flightData.gpsLocation.latitude) + ", " + std::to_string(m_flightData.gpsLocation.longitude);
				ImGui::SetClipboardText(copiedString.c_str());
				ImGui::InsertNotification({ ImGuiToastType_Success, 3000, "The gps coordinate has been copied to your clipboard, paste it in your browser to locate it!" });
				isItemClicked = true;
			}
			ImGui::PopFont();
		}
		ImGui::End();

		if (ImGui::Begin("Barometric & Pressure", nullptr, ImGuiWindowFlags_MenuBar)) {
			static bool showGraphSize = false;
			static bool linky = true;
			static float graphSize = 0.0f;

			if (ImGui::BeginMenuBar()) {
				if (ImGui::BeginMenu("Settings")) {
					if (ImGui::MenuItem("Show Graph Size")) {
						showGraphSize = !showGraphSize;
					}

					if (ImGui::MenuItem("Link Y Axis")) {
						linky = !linky;
					}
					ImGui::SameLine();
					ImGui::Checkbox("##LinkY", &linky);

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			if (showGraphSize)
				ImGui::SliderFloat("Graph size", &graphSize, 0.0f, 500.0f);


			ImGui::Separator();

			auto& graphData = m_graphVariable.bmpGraph;

			{
				float windowYSize = ImGui::GetWindowSize().y;
				float plotFinalSize = (windowYSize / 3) + graphSize;

				static ImPlotRect lims(0, 1, 0, 1);
				ImPlotLegendFlags flags = 0;
				if (ImPlot::BeginPlot("Pressure", ImVec2(-1, plotFinalSize), flags)) {
					ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoTickLabels, 0);
					ImPlot::SetupAxisLimits(ImAxis_X1, t - 10.0f, t, ImPlotCond_Always);
					ImPlot::SetupAxisLinks(ImAxis_Y1, linky ? &lims.Y.Min : nullptr, linky ? &lims.Y.Max : nullptr);
					ImPlot::PlotLine("##Presure", &graphData.bmp.Data[0].x, &graphData.bmp.Data[0].y, graphData.bmp.Data.size(), 0, graphData.bmp.Offset, sizeof(glm::vec4));
					ImPlot::EndPlot();
				}

				{
					ImFont* font = m_imGuiFontMap["LargeFont"];
					ImGui::PushFont(font);

					addTextWithBackground(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, AGC::Utils::clampRGBA(144, 213, 255, 255), "Pressure %.2f", m_flightData.preshure);

					ImGui::PopFont();
				}

				if (ImPlot::BeginPlot("Temprature", ImVec2(-1, plotFinalSize), flags)) {
					ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoTickLabels, 0);
					ImPlot::SetupAxisLimits(ImAxis_X1, t - 10.0f, t, ImPlotCond_Always);
					ImPlot::SetupAxisLinks(ImAxis_Y1, linky ? &lims.Y.Min : nullptr, linky ? &lims.Y.Max : nullptr);
					ImPlot::PlotLine("##Temprature", &graphData.bmp.Data[0].x, &graphData.bmp.Data[0].z, graphData.bmp.Data.size(), 0, graphData.bmp.Offset, sizeof(glm::vec4));
					ImPlot::EndPlot();
				}

				{
					ImFont* font = m_imGuiFontMap["LargeFont"];
					ImGui::PushFont(font);

					addTextWithBackground(glm::vec4{ 1.0f }, AGC::Utils::hexToRGBA("#180161"), "Temprature %.2f", m_flightData.temprature);

					ImGui::PopFont();
				}

				if (ImPlot::BeginPlot("Altitude & Apogee", ImVec2(-1, plotFinalSize), flags)) {
					ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoTickLabels, 0);
					ImPlot::SetupAxisLimits(ImAxis_X1, t - 10.0f, t, ImPlotCond_Always);
					ImPlot::SetupAxisLinks(ImAxis_Y1, linky ? &lims.Y.Min : nullptr, linky ? &lims.Y.Max : nullptr);
					ImPlot::PlotLine("Altitude", &graphData.bmp.Data[0].x, &graphData.bmp.Data[0].w, graphData.bmp.Data.size(), 0, graphData.bmp.Offset, sizeof(glm::vec4));
					ImPlot::PlotLine("Apogee", &m_graphVariable.apogeeGraph.apogee.Data[0].x, &m_graphVariable.apogeeGraph.apogee.Data[0].y, m_graphVariable.apogeeGraph.apogee.Data.size(), 0, m_graphVariable.apogeeGraph.apogee.Offset, sizeof(glm::vec2));
					ImPlot::EndPlot();
				}

				{
					ImFont* font = m_imGuiFontMap["LargeFont"];
					ImGui::PushFont(font);

					addTextWithBackground(glm::vec4{ 1.0f }, AGC::Utils::hexToRGBA("#4F1787"), "Altitude %.2f", m_flightData.altitude);

					ImGui::PopFont();
				}
			
			}

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
				ImGui::Text("Max Queue Size: %d", m_serial->maxQueueSize()); // TODO: Need some fix ??
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
						ss << L"\\\\.\\COM" << m_port;
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

		char commandChar;
		sscanf(str.c_str(), "%c", &commandChar);
		if (commandChar != '$')
			return std::make_pair(str.c_str(), outColor);

		sscanf(str.c_str(), "$%d", &dataType);
		switch (dataType)
		{
		case FlightDataTypeRotation:
		{
			sscanf(str.c_str(), "$%*d %f %f %f", &m_flightData.rotation.x, &m_flightData.rotation.y, &m_flightData.rotation.z);
			out = std::format("Rotation: Roll {} Pitch {} Yaw {}", m_flightData.rotation.x, m_flightData.rotation.y, m_flightData.rotation.z);
			break;
		}

		case FlightDataTypePressure:
		{
			sscanf(str.c_str(), "$%*d %f", &m_flightData.preshure);
			out = std::format("Pressure: {}", m_flightData.preshure);
			break;
		}

		case FlightDataTypeTemprature:
		{
			sscanf(str.c_str(), "$%*d %f", &m_flightData.temprature);
			out = std::format("Temprature: {}", m_flightData.temprature);
			break;
		}

		case FlightDataTypeAltitude:
		{
			sscanf(str.c_str(), "$%*d %f", &m_flightData.altitude);
			out = std::format("Altitude: {}", m_flightData.altitude);
			break;
		}

		case FlightDataTypeHighestAltitude:
		{
			sscanf(str.c_str(), "$%*d %f", &m_flightData.apogee);
			out = std::format("Apogee: {}", m_flightData.apogee);
			break;
		}
			
		case FLightDataTypeGPSLocation:
		{
			sscanf(str.c_str(), "$%*d %lf %lf", &m_flightData.gpsLocation.latitude , &m_flightData.gpsLocation.longitude);
			out = std::format("GPS Location: latitude {}, longitude {}", m_flightData.gpsLocation.latitude, m_flightData.gpsLocation.longitude);
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