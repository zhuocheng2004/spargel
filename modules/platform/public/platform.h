#pragma once

namespace spargel::platform {

class platform {
 public:
  static platform* create();

  virtual ~platform();

  virtual void init() = 0;
  virtual void deinit() = 0;

  virtual bool initialized() = 0;
};

}  // namespace spargel::platform
