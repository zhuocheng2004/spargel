#include <stdlib.h>
#include <time.h>

#include <functional>
#include <vector>

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

  virtual void onMouseMove(float x, float y) {
    for (int i = 0; i < children_.size(); i++) {
      children_[i]->onMouseMove(x, y);
    }
  }

  virtual void onMouseDown(float x, float y) {
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

  void onMouseMove(float x, float y) override {
    if (x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + width_) {
      hovered_ = true;
    } else {
      hovered_ = false;
    }
  }

  void onMouseDown(float x, float y) override {
    if (x >= x_ && x <= x_ + width_ && y >= y_ && y <= y_ + width_) {
      on_click_();
    }
  }

  void render(spargel::ui::Renderer* r) override {
    float render_x = -window()->width() / 2.0 + x_;
    float render_y = window()->height() / 2.0 - y_ - height_;
    if (hovered_) {
      r->drawQuad({{render_x, render_y}, {width_, height_}}, color_);
    } else {
      r->drawQuad({{render_x, render_y}, {width_, height_}},
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
  explicit Editor(spargel::ui::Platform* platform) {
    _renderer = platform->createRenderer();
    _renderer->init();

    _window = platform->createWindow();
    _window->init(500, 500);
    _window->setTitle("Spargel Editor");
    _window->setDelegate(this);

    _renderer->setRenderTarget(_window->renderTarget());

    _root = new Element;
    _root->addChild(new ButtonElement(50, 50, 50, 50, {0, 0, 0}, [this]() {
      auto x = rand() % _window->width();
      auto y = rand() % _window->height();
      _root->addChild(new ButtonElement(
          x, y, 15, 15,
          {sinf(_n * 2.718281 + 36), sinf(_n * 0.577215 * 2 + 136),
           sinf(_n * 3.141592 * 2.718281)},
          [] {}));
      _n++;
    }));
    _root->addChild(new ButtonElement(100, 100, 50, 50, {0, 1, 0}, [this]() {
      _root->removeLast();
      _n--;
    }));

    _root->setWindow(_window);
  }
  ~Editor() { delete _root; }

  void render() override {
    _renderer->begin();
    _root->render(_renderer);
    _renderer->end();
  }

  void onMouseMove(float x, float y) override {
    _root->onMouseMove(x, _window->height() - y);
  }

  void onMouseDown(float x, float y) override {
    _root->onMouseDown(x, _window->height() - y);
  }

 private:
  spargel::ui::Renderer* _renderer;
  spargel::ui::Window* _window;
  Element* _root;
  int _n = 1;
};

int main() {
  srand(time(nullptr));

  auto platform = spargel::ui::Platform::create();
  platform->init();

  Editor editor(platform);

  platform->run();

  return 0;
}
