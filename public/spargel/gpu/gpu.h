#pragma once

namespace spargel::gpu {

class Adapter;
class BindGroup;
class BindGroupLayout;
class Buffer;
class BufferView;
class CommandBuffer;
class ComputePipeline;
class Device;
class Fence;
class Instance;
class PipelineLayout;
class Queue;
class RenderPipeline;
class Sampler;
class Semaphore;
class ShaderModule;
class Texture;
class TextureView;

enum class BackendKind {
  directx,
  metal,
  vulkan,
};

/// @brief everything gpu starts here
class Instance {
 public:
  virtual ~Instance();

  /// @brief create an instance with default backend
  ///
  /// linux : vulkan
  /// windows : directx
  /// macos : metal
  ///
  /// @retval nullptr if failed
  ///
  static Instance* create();

  /// @brief create an instance with given backend
  /// @reval nullptr if failed
  ///
  static Instance* create(BackendKind backend);

  /// @brief initialize the instance
  ///
  virtual void init() = 0;

  /// @brief deinitialize the instance
  ///
  virtual void deinit() = 0;

  /// @brief create a default device
  ///
  /// on platforms that have default devices, this is the default device
  /// obtained from the platform; otherwise, it is created from the first
  /// adapter
  ///
  /// @retval nullptr if failed
  ///
  virtual Device* createDefaultDevice() = 0;
};

class Device {
 public:
  virtual ~Device();

  virtual Queue* createQueue();

  virtual Buffer* createBuffer();
  virtual Texture* createTexture();


};

}  // namespace spargel::gpu
