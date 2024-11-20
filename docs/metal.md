# Metal API

func MTLCreateSystemDefaultDevice() -> (any MTLDevice)?

protocol MTLDevice {
  func makeCommandQueue()
  func makeCommandQueue(maxCommandBufferCount: Int)
  func makeResidencySet(descriptor: MTLResidencySetDescriptor)
  func makeIOCommandQueue(descriptor: MTLIOCommandQueueDescriptor)
  func makeIOFileHandle(url: URL)
  func makeIOFileHandle(url: URL, compressionMethod: MTLIOCompressionMethod)
  func makeIndirectCommandBuffer(
      descriptor: MTLIndirectCommandBufferDescriptor,
      maxCommandCount: Int,
      options: MTLResourceOptions
  )
  func makeRenderPipelineState(descriptor: MTLRenderPipelineDescriptor)
  func makeRenderPipelineState(
      descriptor: MTLRenderPipelineDescriptor,
      completionHandler: MTLNewRenderPipelineStateCompletionHandler
  )
  func makeRenderPipelineState(
      descriptor: MTLRenderPipelineDescriptor,
      options: MTLPipelineOption
  )
  func makeRenderPipelineState(
      descriptor: MTLRenderPipelineDescriptor,
      options: MTLPipelineOption,
      reflection: MTLAutoreleasedRenderPipelineReflection
  )
  func makeRenderPipelineState(
      descriptor: MTLRenderPipelineDescriptor,
      options: MTLPipelineOption,
      completionHandler: MTLNewRenderPipelineStateWithReflectionCompletionHandler
  )
  func makeRenderPipelineState(
      descriptor: MTLMeshRenderPipelineDescriptor,
      options: MTLPipelineOption
  )
  func makeRenderPipelineState(
      descriptor: MTLMeshRenderPipelineDescriptor,
      options: MTLPipelineOption,
      completionHandler: MTLNewRenderPipelineStateWithReflectionCompletionHandler
  )
  func makeRenderPipelineState(
      tileDescriptor: MTLTileRenderPipelineDescriptor,
      options: MTLPipelineOption
  )
  func makeRenderPipelineState(
      tileDescriptor descriptor: MTLTileRenderPipelineDescriptor,
      options: MTLPipelineOption,
      reflection: MTLAutoreleasedRenderPipelineReflection
  )
}
