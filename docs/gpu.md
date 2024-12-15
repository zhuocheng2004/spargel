# GPU

[Mapping](https://docs.vulkan.org/guide/latest/decoder_ring.html)

DirectX | Metal | Vulkan | WebGPU
--- | --- | --- | ---
IDXGIFactory | - | VkInstance | navigator.gpu
IDXGIAdapter | - | VkPhysicalDevice | Adapter
ID3D12CommandQueue | MTLCommandQueue | VkQueue | Queue
ID3D12CommandList | MTLCommandBuffer | VkCommandBuffer | CommandBuffer
ID3D12PipelineState | MTLRenderPipelineState | VKGraphicsPipeline | RenderPipeline
ID3D12PipelineState | MTLComputePipelineState | VkComputePipeline | ComputePipeline
? | MTLLibrary | ? | -
? | MTLFunction | VkShaderModule | -
? | MTLBuffer | VkBuffer | Buffer
? | MTLTexture | VkImage | Texture
? | MTLSampler | VkSampler | Sampler
? | - | VkBufferView | -
? | ?MTLTexture | VkImageView | TextureView
? | MTLHeap | ? | -
? | MTLArgumentBuffer | ? | -
? | MTLResidencySet | ? | -

## Vulkan Presentation

> Calls to `vkQueuePresentKHR` *may* block, but *must* return in finite time.

## Metal

## Related

### Metal

- [Metal Game Window](https://developer.apple.com/documentation/metal/managing_your_game_window_for_metal_in_macos?language=objc)
- [Custom Metal View](https://developer.apple.com/documentation/metal/onscreen_presentation/creating_a_custom_metal_view?language=objc)

### WebGPU

- [WebGPU Bindless Proposal](https://hackmd.io/PCwnjLyVSqmLfTRSqH0viA?view)
- [WebGPU Next](https://developer.chrome.com/blog/next-for-webgpu)

### Projects

- [Rend3](https://github.com/BVE-Reborn/rend3)
- [kohi](https://github.com/travisvroman/kohi)
