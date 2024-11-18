#pragma once

namespace spargel::ui {

class Renderer;
class Window;

enum class PlatformBackend {
  appkit,
  glfw,
  wayland,
  win32,
  xcb,
};

class Platform {
 public:
  // create a platform instance with the backend chosen automatically
  static Platform* create();
  // create a platform instance with the given backend
  static Platform* create(PlatformBackend backend);

  virtual ~Platform();

  // perform platform initialization
  virtual void init() = 0;

  PlatformBackend backend() const;

  // start the platform main loop
  virtual void run() = 0;

  Renderer* createRenderer();
  Window* createWindow();

 protected:
  explicit Platform(PlatformBackend backend);

 private:
  PlatformBackend backend_;
};

}  // namespace spargel::ui
