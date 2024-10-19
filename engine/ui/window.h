#ifndef SPARGEL_ENGINE_UI_WINDOW_H_
#define SPARGEL_ENGINE_UI_WINDOW_H_

namespace spargel::ui {

class WindowDelegate;

class Window {
  public:
    Window(WindowDelegate* delegate) noexcept;

    Window(Window const&) = delete;
    Window& operator=(Window const&) = delete;

    ~Window() noexcept;

    auto GetTitle() const noexcept -> char const*;
    auto SetTitle(char const* title) noexcept -> void;
    
  private:
    WindowDelegate* delegate_;

    char const* title_ = nullptr;
};

}

#endif
