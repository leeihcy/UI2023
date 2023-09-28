#if defined(UI_UNITTEST)

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <iostream>

namespace {
static int main02() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::cout << extensionCount << " extensions supported\n";

  glm::mat4 matrix;
  glm::vec4 vec;
  auto test = matrix * vec;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();

  return 0;
}

}
void tutorial_02() {
  main02();
}

#endif