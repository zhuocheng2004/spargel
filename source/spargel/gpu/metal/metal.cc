#include <spargel/base/allocator.h>
#include <spargel/gpu/metal/metal.h>

namespace spargel::gpu {

Instance* createMetalInstance() {
  return base::Allocator::global()->allocateTyped<InstanceMTL>();
}

void InstanceMTL::init() {}

void InstanceMTL::deinit() {}

DeviceMTL* InstanceMTL::createDefaultDevice() { return nullptr; }

}  // namespace spargel::gpu
