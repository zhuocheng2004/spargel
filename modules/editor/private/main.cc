#include <functional>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include "modules/ui/public/platform.h"
#include "modules/ui/public/renderer.h"
#include "modules/ui/public/window.h"

class element {
 public:
  element() = default;
  virtual ~element() {
    for (auto child : children_) {
      delete child;
    }
  }
  void set_window(spargel::ui::window* window) {
    window_ = window;
    for (auto child : children_) {
      child->set_window(window);
    }
  }

  void add_child(element* child) {
    children_.push_back(child);
    child->set_window(window_);
  }
  void remove_last() {
    auto child = children_.back();
    children_.pop_back();
    delete child;
  }

  virtual void on_mouse_move(double x, double y) {
    for (int i = 0; i < children_.size(); i++) {
      children_[i]->on_mouse_move(x, y);
    }
  }

  virtual void on_mouse_down(double x, double y) {
    for (int i = 0; i < children_.size(); i++) {
      children_[i]->on_mouse_down(x, y);
    }
  }

  virtual void render(spargel::ui::renderer* r) {
    for (auto child : children_) {
      child->render(r);
    }
  }

  spargel::ui::window* window() { return window_; }

 private:
  std::vector<element*> children_;
  spargel::ui::window* window_;
};

class button_element : public element {
 public:
  // origin is upper left
  explicit button_element(float x, float y, float width, float height,
                          spargel::ui::color3 color,
                          std::function<void()> on_click)
      : x_{x},
        y_{y},
        width_{width},
        height_{height},
        color_{color},
        hovered_{false},
        on_click_(std::move(on_click)) {}

  void on_mouse_move(double x, double y) override {
    if (x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + width_) {
      hovered_ = true;
    } else {
      hovered_ = false;
    }
  }

  void on_mouse_down(double x, double y) override {
    if (x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + width_) {
      on_click_();
    }
  }

  void render(spargel::ui::renderer* r) override {
    auto render_x = -window()->width() / 2.0 + x_;
    auto render_y = window()->height() / 2.0 - y_ - height_;
    if (hovered_) {
      r->draw_quad(render_x, render_y, width_, height_, color_);
    } else {
      r->draw_quad(render_x, render_y, width_, height_,
                   {1 - color_.r, 1 - color_.g, 1 - color_.b});
    }
  }

 private:
  float x_;
  float y_;
  float width_;
  float height_;
  spargel::ui::color3 color_;
  bool hovered_;

  std::function<void()> on_click_;
};

class editor final : public spargel::ui::window_delegate {
 public:
  editor() {
    root_ = new element;
    root_->add_child(new button_element(50, 50, 50, 50, {0, 0, 0}, [this]() {
      auto x = rand() % window_->width();
      auto y = rand() % window_->height();
      root_->add_child(new button_element(
          x, y, 15, 15,
          {sinf(n_ * 2.718281 + 36), sinf(n_ * 0.577215 * 2 + 136),
           sinf(n_ * 3.141592 * 2.718281)},
          [] {}));
      n_++;
    }));
    root_->add_child(new button_element(100, 100, 50, 50, {0, 1, 0}, [this]() {
      root_->remove_last();
      n_--;
    }));
  }
  ~editor() { delete root_; }

  void render(spargel::ui::renderer* r) override { root_->render(r); }

  void on_mouse_move(double x, double y) override {
    root_->on_mouse_move(x, window_->height() - y);
  }

  void on_mouse_down(double x, double y) override {
    root_->on_mouse_down(x, window_->height() - y);
  }

  void set_window(spargel::ui::window* window) override {
    window_ = window;
    root_->set_window(window);
  }

 private:
  element* root_;
  spargel::ui::window* window_;
  int n_ = 1;
};

int main() {
  srand(time(nullptr));

  auto platform = spargel::ui::platform::create();
  platform->init();

  editor e;

  auto renderer = platform->create_renderer();
  renderer->init();

  auto window = platform->create_window();
  window->init(500, 500);
  window->set_title("Spargel Editor");
  window->set_delegate(&e);
  window->bind_renderer(renderer);

  platform->run();

  return 0;
}
