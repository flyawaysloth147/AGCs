#include "AGCpch.h"

#include "Window.h"

#include "AGCs/Logging/Log.h"

#include <glm/glm.hpp>

namespace AGC {

	Window::Window(int widht, int height, std::string_view name)
	{
		m_attribute = WindowtAttribute{ widht, height, name.data() };

		init();
	}

	Window::~Window()
	{
		shutdown();
	}

	void Window::init()
	{
		glfwInit();

		// Window Creation Hint
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

		// Create Window
		m_window = glfwCreateWindow(m_attribute.width, m_attribute.height, m_attribute.name.c_str(), nullptr, nullptr);
		if (m_window == nullptr) {
			RUNTIME_ERROR("Failed to create GLFW window");
		}

		AGC_INFO("Window created");

		// Create a opengl context
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, &m_attribute);

		// Load GLAD
		int version = gladLoadGL(glfwGetProcAddress);
		if (!version) {
			RUNTIME_ERROR("Failed to load GLAD");
		}

		AGC_INFO("Glad has been loaded with the version: {0}, {1}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

		glm::vec4 clearColor = { 0.01f, 0.01f, 0.01f, 1.0f }; // Set the default clear color
		glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.a);

		glfwSwapInterval(1); // Set the vsync to true
	}

	void Window::shutdown()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void Window::setupEventHandler()
	{
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			glViewport(0, 0, width, height);
			});
	}

}