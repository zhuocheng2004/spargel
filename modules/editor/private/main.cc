#include <functional>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include "modules/ui/public/platform.h"
#include "modules/ui/public/renderer.h"
#include "modules/ui/public/window.h"

class Element {
 public:
  Element() = default;
  virtual ~Element() {
    for (auto child : children_) {
      delete child;
    }
  }
  void setWindow(spargel::ui::Window* window) {
    window_ = window;
    for (auto child : children_) {
      child->setWindow(window);
    }
  }

  void addChild(Element* child) {
    children_.push_back(child);
    child->setWindow(window_);
  }
  void removeLast() {
    auto child = children_.back();
    children_.pop_back();
    delete child;
  }

  virtual void onMouseMove(double x, double y) {
    for (int i = 0; i < children_.size(); i++) {
      children_[i]->onMouseMove(x, y);
    }
  }

  virtual void onMouseDown(double x, double y) {
    for (int i = 0; i < children_.size(); i++) {
      children_[i]->onMouseDown(x, y);
    }
  }

  virtual void render(spargel::ui::Renderer* r) {
    for (auto child : children_) {
      child->render(r);
    }
  }

  spargel::ui::Window* window() { return window_; }

 private:
  std::vector<Element*> children_;
  spargel::ui::Window* window_;
};

class ButtonElement : public Element {
 public:
  // origin is upper left
  explicit ButtonElement(float x, float y, float width, float height,
                          spargel::ui::Color3 color,
                          std::function<void()> on_click)
      : x_{x},
        y_{y},
        width_{width},
        height_{height},
        color_{color},
        hovered_{false},
        on_click_(std::move(on_click)) {}

  void onMouseMove(double x, double y) override {
    if (x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + width_) {
      hovered_ = true;
    } else {
      hovered_ = false;
    }
  }

  void onMouseDown(double x, double y) override {
    if (x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + width_) {
      on_click_();
    }
  }

  void render(spargel::ui::Renderer* r) override {
    auto render_x = -window()->width() / 2.0 + x_;
    auto render_y = window()->height() / 2.0 - y_ - height_;
    if (hovered_) {
      r->drawQuad(render_x, render_y, width_, height_, color_);
    } else {
      r->drawQuad(render_x, render_y, width_, height_,
                   {1 - color_.r, 1 - color_.g, 1 - color_.b});
    }
  }

 private:
  float x_;
  float y_;
  float width_;
  float height_;
  spargel::ui::Color3 color_;
  bool hovered_;

  std::function<void()> on_click_;
};

class Editor final : public spargel::ui::WindowDelegate {
 public:
  Editor() {
    root_ = new Element;
    root_->addChild(new ButtonElement(50, 50, 50, 50, {0, 0, 0}, [this]() {
      auto x = rand() % window_->width();
      auto y = rand() % window_->height();
      root_->addChild(new ButtonElement(
          x, y, 15, 15,
          {sinf(n_ * 2.718281 + 36), sinf(n_ * 0.577215 * 2 + 136),
           sinf(n_ * 3.141592 * 2.718281)},
          [] {}));
      n_++;
    }));
    root_->addChild(new ButtonElement(100, 100, 50, 50, {0, 1, 0}, [this]() {
      root_->removeLast();
      n_--;
    }));
  }
  ~Editor() { delete root_; }

  void render(spargel::ui::Renderer* r) override { root_->render(r); }

  void onMouseMove(double x, double y) override {
    root_->onMouseMove(x, window_->height() - y);
  }

  void onMouseDown(double x, double y) override {
    root_->onMouseDown(x, window_->height() - y);
  }

  void setWindow(spargel::ui::Window* window) override {
    window_ = window;
    root_->setWindow(window);
  }

 private:
  Element* root_;
  spargel::ui::Window* window_;
  int n_ = 1;
};

int main() {
  srand(time(nullptr));

  auto platform = spargel::ui::Platform::create();
  platform->init();

  Editor editor;

  auto renderer = platform->createRenderer();
  renderer->init();

  auto window = platform->createWindow();
  window->init(500, 500);
  window->setTitle("Spargel Editor");
  window->setDelegate(&editor);
  window->setRenderer(renderer);

  platform->run();

  return 0;
}
