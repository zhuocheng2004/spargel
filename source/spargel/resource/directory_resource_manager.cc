#include "spargel/resource/directory_resource_manager.h"

#include <stdio.h>

namespace spargel::resource {

class DirectoryResource : public Resource {
 public:
  DirectoryResource(const ResourceID& id, FILE* fp) : Resource(id), _fp(fp) {}

  bool init() {
    if (fseek(_fp, 0, SEEK_END) < 0) return false;
    _size = ftell(_fp);
    if (_size < 0) return false;

    return true;
  }

  size_t size() override { return _size; }

  bool getData(void* dest) override {
    if (_refcount <= 0) return false;

    if (_size == 0) return true;

    if (fseek(_fp, 0, SEEK_SET) < 0) return false;
    if (fread(dest, _size, 1, _fp) != 1) return false;

    return true;
  }

  const void* mapData() override {
    /* TODO: OS-specific */
    return nullptr;
  }

 private:
  FILE* _fp;
  size_t _size;

  void _drop() override {
    fclose(_fp);
    _size = 0;
  }
};

Resource* DirectoryResourceManager::open(const ResourceID& id) {
  // FIXME
  const std::string& prefix = _base.empty() ? "" : _base + '/';
  const std::string& path = _use_namespace
                                ? prefix + id.getNS() + '/' + id.getPath()
                                : prefix + id.getPath();
  /*
  printf(
      "spargel::resource::DirectoryResourceManager::open : want to open %s \n",
      path.c_str());
  */
  FILE* fp = fopen(path.c_str(), "rb");
  if (!fp) return nullptr;

  auto resource = new DirectoryResource(id, fp);
  if (!resource->init()) {
    delete resource;
    return nullptr;
  }

  return resource;
}

}  // namespace spargel::resource
