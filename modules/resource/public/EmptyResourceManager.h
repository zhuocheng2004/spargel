#pragma once

#include <modules/resource/public/resource.h>

namespace spargel::resource {

class EmptyResourceManager : public ResourceManager {
 public:
  void close() {}

  bool has(const ResourceID& id) { return false; }

  Resource* open(const ResourceID& id) { return nullptr; }
};

}  // namespace spargel::resource
