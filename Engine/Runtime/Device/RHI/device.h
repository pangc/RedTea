#pragma once
#include "resource.h"
#include "rhi.h"

namespace redtea
{
namespace device
{
	class IDevice
	{
	public:
		virtual bool InitDevice(void* windows) = 0;
		virtual SwapChainHandle CreateSwapchain() = 0;
		virtual HeapHandle CreateHeap(const HeapDesc& d) = 0;
		virtual TextureHandle CreateTexture(const TextureDesc& d) = 0;
		virtual SamplerStateHandle CreateSampler(const SamplerDesc& d) = 0;
		virtual VertexBufferHandle CreateVertexBuffer(const BufferDesc &d) = 0;
		virtual IndexBufferHandle CreateIndexBuffer(const BufferDesc &d) = 0;
		virtual InputLayoutHandle CreateInputLayout(std::vector<VertexAttributeDesc> descs) = 0;
		virtual ShaderHandle CreateShader(const ShaderDesc& d) = 0;
		virtual FrameBufferHandle CreateFrameBuffer() = 0;
		virtual PipelineStateHandle CreatePiplelineState(const PipelineDesc& d) = 0;
		virtual ComputePipelineState CreateComputePipelineState(const ComputePipelineDesc& d) = 0;
	};
}
}