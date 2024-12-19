# GPU

[Mapping](https://docs.vulkan.org/guide/latest/decoder_ring.html)

| DirectX             | Metal                   | Vulkan             | WebGPU          |
| ------------------- | ----------------------- | ------------------ | --------------- |
| IDXGIFactory        | -                       | VkInstance         | navigator.gpu   |
| IDXGIAdapter        | -                       | VkPhysicalDevice   | Adapter         |
| ID3D12CommandQueue  | MTLCommandQueue         | VkQueue            | Queue           |
| ID3D12CommandList   | MTLCommandBuffer        | VkCommandBuffer    | CommandBuffer   |
| ID3D12PipelineState | MTLRenderPipelineState  | VKGraphicsPipeline | RenderPipeline  |
| ID3D12PipelineState | MTLComputePipelineState | VkComputePipeline  | ComputePipeline |
| ?                   | MTLLibrary              | ?                  | -               |
| ?                   | MTLFunction             | VkShaderModule     | -               |
| ?                   | MTLBuffer               | VkBuffer           | Buffer          |
| ?                   | MTLTexture              | VkImage            | Texture         |
| ?                   | MTLSampler              | VkSampler          | Sampler         |
| ?                   | -                       | VkBufferView       | -               |
| ?                   | ?MTLTexture             | VkImageView        | TextureView     |
| ?                   | MTLHeap                 | ?                  | -               |
| ?                   | MTLArgumentBuffer       | ?                  | -               |
| ?                   | MTLResidencySet         | ?                  | -               |

## Vulkan Presentation

> Calls to `vkQueuePresentKHR` _may_ block, but _must_ return in finite time.

## Metal

You can synchronize resources with these mechanisms, which are in ascending scope order:
- Memory barriers
- Memory fences
- Metal events
- Metal shared events


## Related

### Metal

-   [Metal Game Window](https://developer.apple.com/documentation/metal/managing_your_game_window_for_metal_in_macos?language=objc)
-   [Custom Metal View](https://developer.apple.com/documentation/metal/onscreen_presentation/creating_a_custom_metal_view?language=objc)

### WebGPU

-   [WebGPU Bindless Proposal](https://hackmd.io/PCwnjLyVSqmLfTRSqH0viA?view)
-   [WebGPU Next](https://developer.chrome.com/blog/next-for-webgpu)

### Projects

-   [Rend3](https://github.com/BVE-Reborn/rend3)
-   [kohi](https://github.com/travisvroman/kohi)

## xxx

```

    //     fragment: {
    //         function: fragment_shader,
    //         targets: [
    //             {
    //                 format: bgra8_unorm, // enum texture_format
    //                 write_mask: r | g | b | a, // bitmask write_mask
    //                 blend: {
    //                     color: {
    //                         operation: add, // enum blend_action
    //                         src_factor: one, // enum blend_factor
    //                         dst_factor: zero, // enum blend_factor
    //                     },
    //                     alpha: {
    //                         operation: add, // enum blend_action
    //                         src_factor: one, // enum blend_factor
    //                         dst_factor: zero, // enum blend_factor
    //                     },
    //                 },
    //             },
    //         ]
    //     },
    //     depth_stencil: {
    //         depth_compare: less, // enum depth_compare,
    //         depth_write_enable: true,
    //         // todo
    //     }
    // }
    //
```
