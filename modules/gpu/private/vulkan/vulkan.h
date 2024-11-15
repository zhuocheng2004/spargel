#pragma once

#include "modules/gpu/public/gpu.h"

namespace spargel::gpu {

class adapter_vk;
class bind_group_vk;
class bind_group_layout_vk;
class buffer_vk;
class buffer_view_vk;
class command_buffer_vk;
class compute_pipeline_vk;
class device_vk;
class fence_vk;
class instance_vk;
class pipeline_layout_vk;
class queue_vk;
class render_pipeline_vk;
class sampler_vk;
class semaphore_vk;
class shader_module_vk;
class texture_vk;
class texture_view_vk;

class instance_vk : public instance {};

instance* create_vulkan_instance(instance_descriptor const& desc);

}  // namespace spargel::gpu
