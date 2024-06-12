#include "AGCpch.h"

#include "Serial.h"

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
		m_mutex.lock();
		if (m_dataQueue.empty()) {
			m_mutex.unlock();
			return "";
		}

		// Need more code
		std::string data = m_dataQueue.front();
		m_dataQueue.erase(m_dataQueue.begin());
		m_mutex.unlock();

		return data;
	}

	// IMPORTANT:
	// This function needs more codes
	// it will break because the worker thread 
	// is not shutdown and recreated before 
	// closing and opening the connection
	void SerialInterface::recreateConnection(int baudRate, std::wstring port, int timeout, int parity)
	{
		m_baudRate	=	baudRate;
		m_port		=	port;
		m_timeout	=	timeout;
		m_parity	=	parity;

		closeConnection();
		openConnection();
	}

	bool SerialInterface::available()
	{
		m_mutex.lock();
		bool status = !m_dataQueue.empty();
		m_mutex.unlock();

		return status;
	}

	void SerialInterface::init()
	{
		openConnection();
		m_fethcerWorker = std::thread(&SerialInterface::getData, this, m_jobDone);
	}

	void SerialInterface::shutdown()
	{
		m_mutex.lock();
		m_jobDone = true;
		m_mutex.unlock();

		if(m_fethcerWorker.joinable())
			m_fethcerWorker.join();

		closeConnection();
	}

	void SerialInterface::openConnection() 
		// TODO: need to replace assert with something more calm like log errors and not runtime error
	{
		m_hSerial = CreateFile(
			m_port.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0
		);

		AGC_ASSERT(m_hSerial == INVALID_HANDLE_VALUE, "Failed to open serial port");

		// configure port params
		DCB dcbSerialParams = { 0 };
		dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

		AGC_ASSERT(!GetCommState(m_hSerial, &dcbSerialParams), "Failed getting the serial port params");

		dcbSerialParams.BaudRate = m_baudRate;
		dcbSerialParams.ByteSize = 8;
		dcbSerialParams.StopBits = ONESTOPBIT;
		dcbSerialParams.Parity = m_parity;

		AGC_ASSERT(!SetCommState(m_hSerial, &dcbSerialParams), "Failed to set serial port params");


		// Set timeouts
		COMMTIMEOUTS timeouts = { 0 };
		timeouts.ReadIntervalTimeout = m_timeout;
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 10;

		AGC_ASSERT(!SetCommTimeouts(m_hSerial, &timeouts), "Failed to set serial port timeouts");
	}

	void SerialInterface::closeConnection()
	{
		if (m_hSerial != INVALID_HANDLE_VALUE) {
			CloseHandle(m_hSerial);
		}
	}

	void SerialInterface::getData(bool done)
	{
		while (!done) {
			std::string result;
			DWORD bytesRead;
			char c = ' ';

			while (true) {
				if (ReadFile(m_hSerial, (LPVOID)&c, 1, &bytesRead, NULL)) {
					if (bytesRead < 0)
						continue;

					if (c == '\n')
						break;
					if (c == '�') {
						result += '\u00B0';
					}
					else {
						result += c;
					}
				}
				else
					std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			m_mutex.lock();
			m_dataQueue.push_back(result);
			m_mutex.unlock();

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}