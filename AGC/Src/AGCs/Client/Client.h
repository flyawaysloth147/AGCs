#pragma once

#include "AGCs/Core/Application.h"

#include <glm/glm.hpp>
#include <cstdarg>

// The format of the serial data will be...
//    int    ...
// DataType Value, ...
// 
// Rotation
//		Datatype	value	value	value
//		  int 1		float	float	float
//	   Ex: 1		 1.5	 3.4	 2.3
// 
// Pressure
//		Datatype	value
//		  int 2		float
//	   Ex: 2		 1.5
// 
// Temprature
//		Datatype	value
//		  int 3		float
//	   Ex: 3		 1.5
// 
// Altitude 
//		Datatype	value
//		  int 4		float
//	   Ex: 4		 1.5
// 
// Apogee
//		Datatype	value
//		  int 4		float
//	   Ex: 5		 1.5
// 
// Payload Seperated
//		Datatype
//		  int 4	
//	   Ex: 6
//	----------------------------->>> if this dataType is detected it will automatically change the value to true
// 
// Parachute Deployed
//		Datatype
//		  int 4	
//	   Ex: 7
//	----------------------------->>> if this dataType is detected it will automatically change the value to true
// 
// [!] The data type must be at the first of the sent data [!]
// [!] if the first word of the text is not an integer or  [!]
// [!] not in the data type range it will just be printed  [!]
// [!]				with an error message				   [!]


namespace Client {
	// ImGui helper Struct
	struct ScrollingBuffer {
		int MaxSize;
		int Offset;
		ImVector<glm::vec2> Data;

		ScrollingBuffer(int max_size = 1500) {
			MaxSize = max_size;
			Offset = 0;
			Data.reserve(MaxSize);
		}
		void AddPoint(float x, float y) {
			if (Data.size() < MaxSize)
				Data.push_back(glm::vec2(x, y));
			else {
				Data[Offset] = glm::vec2(x, y);
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
		void consoleAddLine(std::string& line, glm::vec4 textColor = glm::vec4(1.0f));
		void VerticalSeparator(float height);
		std::pair<std::string, glm::vec4> parseData(std::string& str);
		void addTextWithBackground(const glm::vec4& textColor, const glm::vec4& backgroundColor, const char* text, ...);

	private:
		AGC::SerialInterface* m_serial = nullptr;
		FlightData m_flightData;
		int m_baudRate = 9600;
		int m_port = 3;
		float m_deltaTime = 0.0f;

	private: // ImGui variable
		bool m_showDemoWindow = true;
		bool m_ImPlotShowDemo = true;
		bool m_reconnectButtonDisable = true;
		bool m_consoleShowRaw = false;	

		bool m_imGuiShowConsole = true;
		std::vector<std::pair<std::string, glm::vec4>> m_consoleBuffer;
		std::unordered_map<std::string, ImFont*> m_imGuiFontMap;

	private: // ImPlot variable
		struct GraphVariable
		{
			float SampleLenght = 10.0f;
			struct RotationGraph {
				ScrollingBuffer roll;
				ScrollingBuffer pitch;
				ScrollingBuffer yaw;
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