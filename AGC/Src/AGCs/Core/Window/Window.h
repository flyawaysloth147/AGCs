#pragma once

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <string>

namespace AGC {
	struct WindowtAttribute
	{
		int width;
		int height;
		std::string name;
	};

	class Window {
	public:
		Window(int widht, int height, std::string_view name);
		~Window();

		inline GLFWwindow* getWindow()		{ return m_window; }
		inline int getWidth()				{ return m_attribute.width; }
		inline int getHeight()				{ return m_attribute.height; }
		inline const char* getName()		{ return m_attribute.name.c_str(); }

		inline void pollEvent() { glfwPollEvents(); }
		inline void clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
		inline void swapBuffer() { glfwSwapBuffers(m_window); }
		inline bool shouldClose() { return glfwWindowShouldClose(m_window); }

	private:
		void init();
		void shutdown();
		
		void setupEventHandler();

	private:
		GLFWwindow* m_window = nullptr;
		WindowtAttribute m_attribute;
	};
}