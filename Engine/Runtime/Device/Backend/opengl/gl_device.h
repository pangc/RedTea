#pragma once
#include "../../RHI/device.h"
#include "gl_platform.h"
#include <unordered_set>

namespace redtea
{
namespace device
{
	class GLSDevice : public IDevice
	{
	public:
		virtual ~GLSDevice();
		virtual bool InitDevice(void* windows) override;
		virtual GraphicsAPI GetGraphicsAPI() override { return GraphicsAPI::GLES3; }
		virtual SwapChainHandle CreateSwapchain(const SwapChainDesc& d) override;
		virtual void MakeCurrent(SwapChainHandle draw, SwapChainHandle read) override;
		virtual HeapHandle CreateHeap(const HeapDesc& d) override;
		virtual TextureHandle CreateTexture(const TextureDesc& d) override;
		virtual SamplerStateHandle CreateSampler(const SamplerDesc& d) override;
		virtual VertexBufferHandle CreateVertexBuffer(const BufferDesc &d) override;
		virtual IndexBufferHandle CreateIndexBuffer(const BufferDesc &d) override;
		virtual InputLayoutHandle CreateInputLayout(std::vector<VertexAttributeDesc> descs) override;
		virtual ShaderHandle CreateShader(const ShaderDesc& d) override;
		virtual FrameBufferHandle CreateFrameBuffer() override;
		virtual PipelineStateHandle CreatePiplelineState(const PipelineDesc& d) override;
		virtual ComputePipelineState CreateComputePipelineState(const ComputePipelineDesc& d) override;
	private:
		void InitializeGlExtensions();
		std::unordered_set<std::string> glExtensions;
		GLPlatform* platform;
	};
}
}