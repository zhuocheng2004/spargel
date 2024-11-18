#pragma once

#include "spargel/resource/resource.h"

namespace spargel::resource {

class EmptyResourceManager : public ResourceManager {
 public:
  void close() {}

  bool has(const ResourceID& id) { return false; }

  Resource* open(const ResourceID& id) { return nullptr; }
};

}  // namespace spargel::resource
