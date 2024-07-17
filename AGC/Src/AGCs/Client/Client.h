#pragma once

#include "AGCs/Core/Application.h"
#include <glm/glm.hpp>

// The format of the serial data will be
//    int    ...
// DataType Value, ...
// 
// Rotation
//		Datatype	value	value	value
//		  int		float	float	float
//	   Ex: 1		 1.5	 3.4	 2.3
// 
// [!] The data type must be at the first of the sent data [!]
// [!] if the first word of the text is not an integer or  [!]
// [!] not in the data type range it will just be printed  [!]
// [!]				with an error message				   [!]


namespace Client {
	// ImGui helper Struct
	struct RollingBuffer {
		float Span;
		ImVector<glm::vec4> Data;
		RollingBuffer() {
			Span = 10.0f;
			Data.reserve(2000);
		}
		void AddPoint(float x, float y, float z, float w) {
			float xmod = fmodf(x, Span);
			if (!Data.empty() && xmod < Data.back().x)
				Data.shrink(0);
			Data.push_back(glm::vec4(xmod, y, z, w));
		}
	};

	struct ScrollingBuffer {
		int MaxSize;
		int Offset;
		ImVector<glm::vec4> Data;

		ScrollingBuffer(int max_size = 2000) {
			MaxSize = max_size;
			Offset = 0;
			Data.reserve(MaxSize);
		}
		void AddPoint(float x, float y, float z, float w) {
			if (Data.size() < MaxSize)
				Data.push_back(glm::vec4(x, y, z, w));
			else {
				Data[Offset] = glm::vec4(x, y, z, w);
				Offset = (Offset + 1) % MaxSize;
			}
		}
		void Erase() {
			if (Data.size() > 0) {
				Data.shrink(0);
				Offset = 0;
			}
		}
	};

	struct FlightData {
		glm::vec3 rotation{ 1.0f }; // Roll, Pitch, Yaw
		float presure = 0.0f;
		float temprature = 0.0f;
		float altitude = 0.0f;
		float apogee = 0.0f;
		bool isPayloadSeparated = false;
		bool isParachuteDeployed = false;
	};

	enum FlightDataType { 
		None = 0,
		FlightDataTypeRotation,
		FlightDataTypePressure,
		FlightDataTypeTemprature,
		FlightDataTypeAltitude,
		FlightDataTypeHighestAltitude,
		FlightDataTypePayloadSeperated,
		FlightDataTypeParachuteDeployed
	};

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
		std::string parseData(std::string& str);

	private:
		AGC::SerialInterface* m_serial = nullptr;
		FlightData m_flightData;
		int m_baudRate = 9600;
		int m_port = 3;
		float m_deltaTime;

	private: // ImGui variable
		bool m_showDemoWindow = true;
		bool m_ImPlotShowDemo = true;
		bool m_showConsole = true;
		bool m_reconnectButtonDisable = true;
		std::vector<std::string> m_consoleBuffer;

	private: // ImPlot variable
		struct GraphVariable
		{
			float SampleLenght = 10.0f;
			struct RotationGraph {
				ScrollingBuffer rotation; // x = time, y = roll, z = pitch, w = yaw
				bool showGraph = true;
			}; RotationGraph rotationGraph;
		};

		GraphVariable m_graphVariable;
	};

	class App : public AGC::Application {
	public:
		App()
			: Application(700, 800, "Axantera Ground Control system") {
			pushLayer(new MainLayer());
		}
	};
}