#include "spargel/resource/directory_resource_manager.h"

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
  return fp ? new DirectoryResource(id, fp) : nullptr;
}

}  // namespace spargel::resource
