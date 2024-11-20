#pragma once

#include "spargel/gpu/gpu.h"

namespace spargel::gpu {

class DeviceMTL;

class DeviceMTL final : public Device {};

class InstanceMTL final : public Instance {
 public:
  InstanceMTL();
  ~InstanceMTL() override;

  void init() override;
  void deinit() override;

  DeviceMTL* createDefaultDevice() override;
};

Instance* createMetalInstance();

}  // namespace spargel::gpu
