#pragma once

#include <spargel/gpu/gpu.h>

struct sgpu_operations {
  int (*create_instance)(struct sgpu_instance_descriptor const*,
                         sgpu_instance_id*);
  void (*destroy_instance)(sgpu_instance_id);
  int (*create_default_device)(sgpu_instance_id, sgpu_device_id*);
  void (*destroy_device)(sgpu_device_id);
};
