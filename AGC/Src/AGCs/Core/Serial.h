#pragma once

#include "AGCs/Logging/Log.h"
#include "AGCs/Utils/Utils.h"

#include <string>
#include <vector>
#include <windows.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <codecvt>

namespace AGC {
	class SerialInterface {
	public:

		SerialInterface(int baudRate, std::wstring port, int timeout, int parity);
		~SerialInterface();

		std::string fetchData();
		void recreateConnection(int baudRate, std::wstring port, int timeout, int parity);

		bool available();
		bool opened() const { return m_connectionOpened; }
		unsigned int maxQueueSize(); 
		unsigned int currentQueueSize();
		int getParity() const { return m_parity; }
		int getBaudRate() const { return m_baudRate; }
		std::string getPort() const { std::wstring_convert<std::codecvt_utf8<wchar_t>> converter; return converter.to_bytes(m_port); }

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

		// Shared resources
		std::vector<std::string> m_dataQueue;
		bool m_connectionOpened = false;
		bool m_jobDone = false;

		std::thread m_fethcerWorker;
		std::mutex m_mutex;
	};
}