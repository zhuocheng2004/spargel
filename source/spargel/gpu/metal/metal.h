#pragma once

#include "spargel/gpu/gpu.h"

namespace spargel::gpu {

class adapter_mtl;
class bind_group_mtl;
class bind_group_layout_mtl;
class buffer_mtl;
class buffer_view_mtl;
class command_buffer_mtl;
class compute_pipeline_mtl;
class device_mtl;
class fence_mtl;
class instance_mtl;
class pipeline_layout_mtl;
class queue_mtl;
class render_pipeline_mtl;
class sampler_mtl;
class semaphore_mtl;
class shader_module_mtl;
class texture_mtl;
class texture_view_mtl;

class instance_mtl : public instance {};

instance* create_metal_instance(instance_descriptor const& desc);

}  // namespace spargel::gpu
