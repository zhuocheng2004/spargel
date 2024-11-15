#pragma once

namespace spargel::gpu {

class adapter;
class bind_group;
class bind_group_layout;
class buffer;
class buffer_view;
class command_buffer;
class compute_pipeline;
class device;
class fence;
class instance;
class pipeline_layout;
class queue;
class render_pipeline;
class sampler;
class semaphore;
class shader_module;
class texture;
class texture_view;

enum class backend_kind {
  directx,
  metal,
  vulkan,
};

struct instance_descriptor;

struct instance_descriptor {
  backend_kind backend;
};

class instance {
 public:
  static instance* create(instance_descriptor const& desc);

  virtual ~instance();
};

}  // namespace spargel::gpu
