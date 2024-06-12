#include "AGCpch.h"

#include "AGCs/Core/Application.h"

#ifdef BUILD_WINDOWS

int main() {
	AGC::Log::init();

	AGC::Application* app = new AGC::Application(800, 800, "AGC Axantera Ground Control");

	app->run();

	delete app;

	return 0;
}

#endif // BUILD_WINDOWS
