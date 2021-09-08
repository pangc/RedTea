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
	typedef RefCountPtr<ISwapChain> SwapChainHandle;

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
	public:
		virtual ~IHeap() {}
		HeapDesc& GetDesc() { return desc; };
	private:
		HeapDesc desc;
	};
	typedef RefCountPtr<IHeap> HeapHandler;

	enum class BufferType : uint8_t
	{
		VertexBuffer,
		IndexBuffer
	};

	struct BufferDesc
	{
		BufferType type;
		uint32_t size;
	};

	enum class ResourceFormat : uint32_t
	{
		Unknown,
		R8Unorm,
		R8Snorm,
		R16Unorm,
		R16Snorm,
		RG8Unorm,
		RG8Snorm,
		RG16Unorm,
		RG16Snorm,
		RGB16Unorm,
		RGB16Snorm,
		R24UnormX8,
		RGB5A1Unorm,
		RGBA8Unorm,
		RGBA8Snorm,
		RGB10A2Unorm,
		RGB10A2Uint,
		RGBA16Unorm,
		RGBA8UnormSrgb,
		R16Float,
		RG16Float,
		RGB16Float,
		RGBA16Float,
		R32Float,
		R32FloatX32,
		RG32Float,
		RGB32Float,
		RGBA32Float,
		R11G11B10Float,
		RGB9E5Float,
		R8Int,
		R8Uint,
		R16Int,
		R16Uint,
		R32Int,
		R32Uint,
		RG8Int,
		RG8Uint,
		RG16Int,
		RG16Uint,
		RG32Int,
		RG32Uint,
		RGB16Int,
		RGB16Uint,
		RGB32Int,
		RGB32Uint,
		RGBA8Int,
		RGBA8Uint,
		RGBA16Int,
		RGBA16Uint,
		RGBA32Int,
		RGBA32Uint,

		BGRA8Unorm,
		BGRA8UnormSrgb,

		BGRX8Unorm,
		BGRX8UnormSrgb,
		Alpha8Unorm,
		Alpha32Float,
		R5G6B5Unorm,

		// Depth-stencil
		D32Float,
		D16Unorm,
		D32FloatS8X24,
		D24UnormS8,

		// Compressed formats
		BC1Unorm,   // DXT1
		BC1UnormSrgb,
		BC2Unorm,   // DXT3
		BC2UnormSrgb,
		BC3Unorm,   // DXT5
		BC3UnormSrgb,
		BC4Unorm,   // RGTC Unsigned Red
		BC4Snorm,   // RGTC Signed Red
		BC5Unorm,   // RGTC Unsigned RG
		BC5Snorm,   // RGTC Signed RG
		BC6HS16,
		BC6HU16,
		BC7Unorm,
		BC7UnormSrgb,

		Count
	};

	class IBuffer : public IResource
	{
	public:
		virtual ~IBuffer() {}
		BufferDesc& GetDescription() { return desc; };
	private:
		BufferDesc desc;
	};
	typedef RefCountPtr<IBuffer> BufferHandler;

//**************************************************************
//**				Vertex									  **
//**************************************************************
	struct VertexAttributeDesc
	{
		std::string name;
		uint32_t arraySize = 1;
		uint32_t bufferIndex = 0;
		uint32_t offset = 0;
		uint8_t stride = 0;

		VertexAttributeDesc& SetName(std::string value) { name = value;  return *this; }
		constexpr VertexAttributeDesc& SetArraySize(uint32_t value) { arraySize = value; return *this; }
		constexpr VertexAttributeDesc& SetBufferIndex(uint32_t value) { bufferIndex = value; return *this; }
		constexpr VertexAttributeDesc& SetOffset(uint32_t value) { offset = value; return *this; }
		constexpr VertexAttributeDesc& setStride(uint32_t value) { stride = value; return *this; }
	};

	class IInputLayout : public IResource
	{
	public:
		std::vector<VertexAttributeDesc> attribtues;
	};
	typedef RefCountPtr<IInputLayout> InputLayoutHandler;

	class IVertexBuffer : public IBuffer
	{
	public:
		virtual ~IVertexBuffer() {}
	};

	class IIndexBuffer : public IBuffer
	{
	public:
		virtual ~IIndexBuffer() {}
	};

//**************************************************************
//**				Texture									  **
//**************************************************************

	enum CpuAccessMode : uint8_t
	{
		None,
		WriteOnly,
		ReadOnly,
		ReadWrite,
	};


	enum class TextureType : uint8_t
	{
		Unknown,
		Texture1D,
		Texture1DArray,
		Texture2D,
		Texture2DArray,
		TextureCube,
		TextureCubeArray,
		Texture2DMS,
		Texture2DMSArray,
		Texture3D
	};

	struct TextureDesc
	{
		uint32_t size = 1;
		uint32_t width = 1;
		uint32_t height = 1;
		uint32_t depth = 1;
		uint32_t arraySize = 1;
		uint32_t mipLevels = 1;
		uint32_t sampleCount = 1;
		uint32_t sampleQuality = 0;
		CpuAccessMode accessMode = CpuAccessMode::None;
		TextureType type = TextureType::Texture2D;
		ResourceFormat format = ResourceFormat::Unknown;


	};

	class ITexture : public IResource
	{
	public:
		TextureDesc& GetDesc() { return desc; }
	private:
		TextureDesc  desc;
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
}
}