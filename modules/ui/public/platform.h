#pragma once

namespace spargel::ui {

class renderer;
class window;

enum class platform_backend {
  appkit,
  glfw,
  wayland,
  win32,
  xcb,
};

class platform {
 public:
  // create a platform instance with the backend chosen automatically
  static platform* create();
  // create a platform instance with the given backend
  static platform* create(platform_backend backend);

  virtual ~platform();

  // perform platform initialization
  virtual void init() = 0;

  platform_backend backend() const;

  // start the platform main loop
  virtual void run() = 0;

  renderer* create_renderer();
  window* create_window();

 protected:
  explicit platform(platform_backend backend);

 private:
  platform_backend backend_;
};

}  // namespace spargel::ui
