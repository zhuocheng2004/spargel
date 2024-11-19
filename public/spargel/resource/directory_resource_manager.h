#pragma once

#include "spargel/resource/resource.h"

namespace spargel::resource {

class DirectoryResourceManager : public ResourceManager {
 public:
  DirectoryResourceManager(const std::string& base, bool use_namespace = false)
      : _base(base), _use_namespace(use_namespace) {}

  void close() {}

  bool has(const ResourceID& id) {
    /* TODO: OS-specific */
    return false;
  }

  Resource* open(const ResourceID& id);

 private:
  const std::string _base;
  const bool _use_namespace;
};

}  // namespace spargel::resource
