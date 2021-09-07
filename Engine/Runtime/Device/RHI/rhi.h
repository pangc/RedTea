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

	enum BufferType
	{
		VertexBuffer,
		IndexBuffer
	};

	struct VertexAttributeDesc
	{
		std::string name;
		uint32_t arraySize = 1;
		uint32_t bufferIndex = 0;
		uint32_t offset = 0;
		uint8_t stride = 0;
	};

	struct IBufferDesc
	{

	};

	class IVertexInputLayout : public IResource
	{
	public:
		std::vector<VertexAttributeDesc> attribtues;

	};

	class IBuffer : public IResource
	{

	};

	class IVertexBuffer : public IBuffer
	{

	};



	class ITexture : public IResource
	{
	};

	typedef RefCountPtr<ISwapChain> SwapChainHandle;
	typedef RefCountPtr<IVertexInputLayout> VertexInputLayoutHandler;
}
}