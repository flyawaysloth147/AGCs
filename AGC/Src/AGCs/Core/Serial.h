#pragma once

#include "AGCs/Logging/Log.h"

#include <string>
#include <vector>
#include <windows.h>
#include <thread>
#include <mutex>
#include <chrono>

namespace AGC {
	class SerialInterface {
	public:

		SerialInterface(int baudRate, std::wstring port, int timeout, int parity);
		~SerialInterface();

		std::string fetchData();
		void recreateConnection(int baudRate, std::wstring port, int timeout, int parity);
		bool available();

	private:
		void init();
		void shutdown();

		void openConnection();
		void closeConnection();
		void getData();

	private:
		HANDLE m_hSerial = nullptr;
		int m_parity = NOPARITY;
		std::wstring m_port;
		int m_timeout = 50; // ms
		int m_baudRate = 9600; 
		std::vector<std::string> m_dataQueue;

		std::thread m_fethcerWorker;
		std::mutex m_mutex;

	};
}