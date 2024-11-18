#pragma once

#include "spargel/gpu/gpu.h"

namespace spargel::gpu {

class adapter_dx;
class bind_group_dx;
class bind_group_layout_dx;
class buffer_dx;
class buffer_view_dx;
class command_buffer_dx;
class compute_pipeline_dx;
class device_dx;
class fence_dx;
class instance_dx;
class pipeline_layout_dx;
class queue_dx;
class render_pipeline_dx;
class sampler_dx;
class semaphore_dx;
class shader_module_dx;
class texture_dx;
class texture_view_dx;

instance* create_directx_instance(instance_descriptor const& desc);

}  // namespace spargel::gpu
