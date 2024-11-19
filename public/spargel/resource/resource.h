#pragma once

// FIXME: Avoid the standard library.
#include <stdint.h>

#include <string>

namespace spargel::resource {

class ResourceID {
 public:
  const std::string NS_DEFAULT = "core";

  ResourceID(const std::string& ns, const std::string& path)
      : _ns(ns), _path(path) {}

  explicit ResourceID(const std::string& path) : ResourceID(NS_DEFAULT, path) {}

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
  /*
   * When creating, reference count is set to one.
   */
  Resource(const ResourceID& id) : _id(id), _refcount(1) {}

  virtual ~Resource() { drop(); }

  const ResourceID& getID() const { return _id; }

  /** reference +1 */
  void get() { _refcount++; }

  /** reference -1
   *
   * The resource will be released after all reference are removed, and is no
   * longer usable.
   */
  void put() {
    _refcount--;
    if (_refcount <= 0) _drop();
  }

  /**
   * immediately remove all references and release the resource
   */
  void drop() {
    _refcount = 0;
    _drop();
  }

  virtual size_t size() = 0;

  /**
   * write resource data to memory
   *
   * @return `true` if the operation is done successfully, otherwise `false`
   */
  virtual bool getData(void* dest) = 0;

  /**
   * obtain a read-only memory map of the resource
   *
   * @return pointer to the mapped memory, or `nullptr` if the mapping cannot be
   * done or is not supported/implemented
   */
  virtual const void* mapData() = 0;

 protected:
  const ResourceID& _id;
  int _refcount;

  /** will be called when the last reference is removed */
  virtual void _drop() = 0;
};

/**
 * a place from where you can open resources
 */
class ResourceManager {
 public:
  /**
   * shut down the resource manager
   *
   * We can no longer ask/open resources from the manager after this operation.
   */
  virtual void close() = 0;

  /**
   * ask if a resource exists
   */
  virtual bool has(const ResourceID& id) = 0;

  /**
   * obtain a handle of the opened resource
   *
   * The reference count of the returned handle is set to one.
   *
   * FIXME: Return the resource content with error handling in the future.
   * Currently: returns nullptr if the resource cannot be opened. When the
   * handle is no longer used, the caller should delete this handle pointer
   * (will automatically release the resource).
   */
  virtual Resource* open(const ResourceID& id) = 0;
};

}  // namespace spargel::resource
