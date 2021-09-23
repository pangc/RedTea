#pragma once
#include "../../RHI/device.h"
#include "gl_platform.h"
#include "gl_context.h"
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
		virtual IndexBufferHandle CreateIndexBuffer(const BufferDesc &d) override;
		virtual ShaderHandle CreateShader(const ShaderDesc& d) override;
		virtual RenderPrimitiveHandle CreateRenderPrimitive() override;
		virtual FrameBufferHandle CreateFrameBuffer() override;
		virtual PipelineStateHandle CreatePiplelineState(const PipelineDesc& d) override;
		virtual ComputePipelineState CreateComputePipelineState(const ComputePipelineDesc& d) override;

		void CreateBufferObject(BufferHandle handle, BufferDesc d, uint32_t size);
		void UpdateBufferObject(BufferHandle handle, uint32_t offset, BufferData &&data);
		void CreateRenderPrimitiveObject(RenderPrimitiveHandle handle);
	private:
		void InitializeGlExtensions();
		std::unordered_set<std::string> glExtensions;
		GLPlatform* platform;
		GLContext mCtx;
	};
}
}