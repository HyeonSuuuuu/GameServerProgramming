#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

namespace lve
{
class LveWindow {
public:
	LveWindow(int w, int h, std::string name);
	~LveWindow();

	LveWindow(const LveWindow&) = delete;
	LveWindow& operator=(const LveWindow&) = delete;

	bool shouldClose() { return glfwWindowShouldClose(window); }
	GLFWwindow* getGLFWwindow() const { return window; } // 방향키
	VkExtent2D getExtent() 
	{
		return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
	}

	bool wasWindowResized() { return framebufferResized; }
	void resetWindowResizedFlag() { framebufferResized = false; }

	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}
private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	void initWindow();

	int width;
	int height;
	bool framebufferResized = false;

	std::string windowName;
	GLFWwindow* window;

};

}