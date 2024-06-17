#include "AGCpch.h"

#include "Serial.h"
#include "AGCs/Utils/Utils.h"

namespace AGC {

	SerialInterface::SerialInterface(int baudRate, std::wstring port, int timeout, int parity)
		: m_baudRate{ baudRate }, m_port{ port }, m_timeout{ timeout }, m_parity{ parity }
	{
		init();
	}

	SerialInterface::~SerialInterface()
	{
		shutdown();
	}

	std::string SerialInterface::fetchData()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_dataQueue.empty()) {
			return "";
		}

		std::string data = m_dataQueue.front();
		m_dataQueue.erase(m_dataQueue.begin());

		return data;
	}

	void SerialInterface::recreateConnection(int baudRate, std::wstring port, int timeout, int parity)
	{
		m_baudRate = baudRate;
		m_port = port;
		m_timeout = timeout;
		m_parity = parity;

		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_jobDone = true;
		}

		if(m_fethcerWorker.joinable())
			m_fethcerWorker.join();	

		closeConnection();
		openConnection();
		
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_jobDone = false;
		}		
		m_fethcerWorker = std::thread(&SerialInterface::getData, this);
	}

	bool SerialInterface::available()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		bool status = !m_dataQueue.empty();

		return status;
	}

	unsigned int SerialInterface::maxQueueSize() {
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_dataQueue.max_size();
	}

	unsigned int SerialInterface::currentQueueSize() {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_dataQueue.size();
	}

	void SerialInterface::init()
	{
		openConnection();
		if(m_connectionOpened)
			m_fethcerWorker = std::thread(&SerialInterface::getData, this);
	}

	void SerialInterface::shutdown()
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_jobDone = true;
		}

		if (m_fethcerWorker.joinable())
			m_fethcerWorker.join();

		closeConnection();
	}

	void SerialInterface::openConnection()
	{
		if (m_connectionOpened) {
			AGC_WARN("serial connection already opened");
			return;
		}

		m_hSerial = CreateFile(
			m_port.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0
		);

		if (m_hSerial == INVALID_HANDLE_VALUE) {
			AGC_ERROR("Failed to open serial port, please check if you are in the right port");
			return;
		}

		// configure port params
		DCB dcbSerialParams = { 0 };
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

		if (!GetCommState(m_hSerial, &dcbSerialParams))
		{
			AGC_ERROR("Failed getting the serial port params");
			return;
		}

		dcbSerialParams.BaudRate = m_baudRate;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = m_parity;

		if (!SetCommState(m_hSerial, &dcbSerialParams)) {
			AGC_ERROR("Failed to set serial port params");
			return;
		}

		// Set timeouts
		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = m_timeout;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;

		if (!SetCommTimeouts(m_hSerial, &timeouts)) {
			AGC_ERROR("Failed to set serial port timeouts");
			return;
		}

		m_connectionOpened = true;
	}

	void SerialInterface::closeConnection()
	{
		if (!m_connectionOpened) {
			AGC_WARN("Serial connection already closed");
		}

		if (m_hSerial != INVALID_HANDLE_VALUE) {
			CloseHandle(m_hSerial);
			m_connectionOpened = false;
		}
	}

	void SerialInterface::getData() // need to handle disconection
	{
		while (true) {
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				if (m_jobDone)
					return;
			}

			std::string result;
			DWORD bytesRead;
			char c = ' ';

			while (true) {
				if (ReadFile(m_hSerial, (LPVOID)&c, 1, &bytesRead, nullptr)) {
					if (bytesRead < 0)
						continue;

					if (c == '\n')
						break;

					result += c;
				}
				else
					std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			{
				std::unique_lock<std::mutex> lock{ m_mutex };
				if (m_dataQueue.size() < 100) {
					m_dataQueue.push_back(Utils::trimLeadingSpaces(result));
				}
				else
				{
					lock.unlock();
					std::this_thread::sleep_for(std::chrono::seconds(3));
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}