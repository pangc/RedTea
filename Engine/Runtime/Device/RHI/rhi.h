#pragma once
#include "resource.h"
#include <vector>

namespace redtea
{
namespace device
{
	class ISwapChain : public IResource
	{
	public:
		virtual void Present() = 0;
		virtual void Resize(uint16_t width, uint16_t height) = 0;
	};

	enum class HeapType : uint8_t
	{
		DeviceLocal,
		Upload,
		Readback
	};

	struct HeapDesc
	{
		std::string name;
		HeapType type;
	};

	class IHeap : public IResource
	{
	};

	enum BufferType : uint8_t
	{
		VertexBuffer,
		IndexBuffer
	};

	struct BufferDesc
	{
		BufferType type;
		uint32_t size;
	};

	class IBuffer : public IResource
	{
	public:
		BufferDesc desc;

	};
//******************************************************
//**		Vertex									  **
//******************************************************
	struct VertexAttributeDesc
	{
		std::string name;
		uint32_t arraySize = 1;
		uint32_t bufferIndex = 0;
		uint32_t offset = 0;
		uint8_t stride = 0;
	};

	class IInputLayout : public IResource
	{
	public:
		std::vector<VertexAttributeDesc> attribtues;

	};

	class IVertexBuffer : public IBuffer
	{

	};

	class IIndexBuffer : public IBuffer
	{

	};

	struct TextureDesc
	{
	};

	class ITexture : public IResource
	{
	};

	struct SamplerDesc
	{
	};

	class ISampler : public IResource
	{

	};

	struct PipelineDesc
	{
	};

	class IPipelineState : public IResource
	{};

	struct ComputePipelineDesc
	{};

	class IComputePipelineState : public IResource
	{
	};

	typedef RefCountPtr<ISwapChain> SwapChainHandle;
	typedef RefCountPtr<IVertexInputLayout> VertexInputLayoutHandler;
}
}