#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void glfw_error_callback(int e, const char* msg) {
  printf("error: glfw: %s\n", msg);
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action,
                       int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

template <typename T>
struct deferred {
  T callback;
  ~deferred() { callback(); }
};

int main() {
  if (!glfwInit()) {
    printf("error: cannot init glfw\n");
    return 1;
  }
  auto glfw_cleanup_ = deferred{[] { glfwTerminate(); }};
  glfwSetErrorCallback(glfw_error_callback);
  auto window = glfwCreateWindow(500, 500, "Spargel Engine", nullptr, nullptr);
  if (!window) {
    printf("error: cannot create window\n");
  }
  auto window_cleanup_ = deferred{[=] { glfwDestroyWindow(window); }};
  glfwSetKeyCallback(window, glfw_key_callback);
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
  return 0;
}
