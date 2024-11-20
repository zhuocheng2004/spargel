#pragma once

#include "spargel/base/types.h"

namespace spargel::base {

// utf-8 string
class String final {
 public:
  String();
  ~String();
};

// c string, i.e. null terminated
class CString final {};

}  // namespace spargel::base
