#pragma once

#include <modules/resource/public/resource.h>
#include <stdio.h>

namespace spargel::resource {

class DirectoryResource : public Resource {
 public:
  DirectoryResource(const ResourceID& id, FILE* fp) : Resource(id), _fp(fp) {
    fseek(fp, 0, SEEK_END);
    _size = ftell(fp);
  }

  void release() { fclose(_fp); }

  size_t size() { return _size; }

  void getData(void* dest) {
    fseek(_fp, 0, SEEK_SET);
    fread(dest, _size, 1, _fp);
  }

  const void* mapData() {
    /* TODO: OS-specific */
    return nullptr;
  }

 private:
  FILE* _fp;
  size_t _size;
};

class DirectoryResourceManager : public ResourceManager {
 public:
  DirectoryResourceManager(const std::string& base) : _base(base) {}

  void close() {}

  bool has(const ResourceID& id) {
    /* TODO: OS-specific */
    return false;
  }

  Resource* open(const ResourceID& id) {
    const std::string& prefix = _base.empty() ? "" : _base + '/';
    printf("==== %s ==== \n", (prefix + id.getPath()).c_str());
    FILE* fp = fopen((prefix + id.getPath()).c_str(), "rb");
    return fp ? new DirectoryResource(id, fp) : nullptr;
  }

 private:
  const std::string _base;
};

}  // namespace spargel::resource
