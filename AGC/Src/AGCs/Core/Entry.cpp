#include "AGCpch.h"

#include "AGCs/Client/Client.h"

#ifdef BUILD_WINDOWS

int main() {
	AGC::Log::init();

	AGC::Application* app = new Client::App();

	app->run();

	delete app;

	return 0;
}

#endif // BUILD_WINDOWS
