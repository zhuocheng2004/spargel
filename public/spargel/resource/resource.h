#pragma once

// FIXME: Avoid the standard library.
#include <stdint.h>

#include <string>

namespace spargel::resource {

class ResourceID {
 public:
  const std::string k_default_ns = "core";

  ResourceID(const std::string& ns, const std::string& path)
      : _ns(ns), _path(path) {}

  explicit ResourceID(const std::string& path) : ResourceID(k_default_ns, path) {}

  bool operator==(const ResourceID& that) {
    return _ns == that._ns && _path == that._path;
  }

  const std::string& getNS() const { return _ns; }

  const std::string& getPath() const { return _path; }

 private:
  const std::string _ns;
  const std::string _path;
};

/**
 * a handle of opened resource
 */
class Resource {
 public:
  Resource(const ResourceID& id) : id(id) {}

  const ResourceID& getID() const { return id; }

  virtual void release() = 0;

  virtual size_t size() = 0;

  /**
   * write resource data to memory
   */
  virtual void getData(void* dest) = 0;

  /**
   * obtain a read-only memory map of the resource
   */
  virtual const void* mapData() = 0;

 protected:
  const ResourceID& id;
};

/**
 * a place from where you can open resources
 */
class ResourceManager {
 public:

  virtual void close() = 0;

  virtual bool has(const ResourceID& id) = 0;

  virtual Resource* open(const ResourceID& id) = 0;
};

}  // namespace spargel::resource
