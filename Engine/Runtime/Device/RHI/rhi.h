#pragma once
#include "resource.h"
#include <vector>

namespace redtea
{
namespace device
{
	enum class GraphicsAPI : uint8_t
	{
		GLES3,
		VLUKAN,
		METAL,
		DX11,
		DX12
	};

	class ISwapChain : public RefCounter<IResource>
	{
	public:
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

	class IHeap : public RefCounter<IResource>
	{
	public:
		virtual ~IHeap() {}
		HeapDesc& GetDesc() { return desc; };
	private:
		HeapDesc desc;
	};
	typedef RefCountPtr<IHeap> HeapHandle;

	enum CpuAccessMode : uint8_t
	{
		None,
		WriteOnly,
		ReadOnly,
		ReadWrite,
	};

	enum class BufferType : uint8_t
	{
		VertexBuffer,
		IndexBuffer
	};

	struct BufferDesc
	{
		BufferType type;
		uint32_t size;
		CpuAccessMode cpuAccess;

		constexpr BufferDesc& SetBufferType(BufferType value) { type = value; return *this; }
		constexpr BufferDesc& SetSize(uint32_t value) { size = value; return *this; }
		constexpr BufferDesc& SetCpuAccess(CpuAccessMode value) { cpuAccess = value; return *this; }
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

	class IBuffer : public RefCounter<IResource>
	{
	public:
		virtual ~IBuffer() {}
		BufferDesc& GetDescription() { return desc; };
	private:
		BufferDesc desc;
	};
	typedef RefCountPtr<IBuffer> BufferHandle;

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

	class IInputLayout : public RefCounter<IResource>
	{
	public:
		std::vector<VertexAttributeDesc> attribtues;
	};
	typedef RefCountPtr<IInputLayout> InputLayoutHandle;

	class IVertexBuffer : public IBuffer
	{
	public:
		virtual ~IVertexBuffer() {}
	};
	typedef RefCountPtr<IVertexBuffer> VertexBufferHandle;


	class IIndexBuffer : public IBuffer
	{
	public:
		virtual ~IIndexBuffer() {}
	};
	typedef RefCountPtr<IIndexBuffer> IndexBufferHandle;

//**************************************************************
//**				Texture									  **
//**************************************************************

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

		constexpr TextureDesc& SetWidth(uint32_t value) { width = value; return *this; }
		constexpr TextureDesc& SetHeight(uint32_t value) { height = value; return *this; }
		constexpr TextureDesc& SetDepth(uint32_t value) { depth = value; return *this; }
		constexpr TextureDesc& SetArraySize(uint32_t value) { arraySize = value; return *this; }
		constexpr TextureDesc& SetMipLevels(uint32_t value) { mipLevels = value; return *this; }
		constexpr TextureDesc& SetSampleCount(uint32_t value) { sampleCount = value; return *this; }
		constexpr TextureDesc& SetSampleQuality(uint32_t value) { sampleQuality = value; return *this; }
		constexpr TextureDesc& SetAccessMode(CpuAccessMode value) { accessMode = value; return *this; }
		constexpr TextureDesc& SetTextureType(TextureType value) { type = value; return *this; }
		constexpr TextureDesc& SetFormat(ResourceFormat value) { format = value; return *this; }

	};

	class ITexture : public RefCounter<IResource>
	{
	public:
		TextureDesc& GetDesc() { return desc; }
	private:
		TextureDesc  desc;
	};
	typedef RefCountPtr<ITexture> TextureHandle;

	enum class SamplerAddressMode : uint8_t
	{
		Clamp,
		Wrap,
		Border,
		Mirror,
		MirrorOnce,
	};

	struct SamplerDesc
	{
		bool minFilter = true;
		bool magFilter = true;
		bool mipFilter = true;

		SamplerAddressMode addressU = SamplerAddressMode::Clamp;
		SamplerAddressMode addressV = SamplerAddressMode::Clamp;
		SamplerAddressMode addressW = SamplerAddressMode::Clamp;

		SamplerDesc& SetMinFilter(bool enable) { minFilter = enable; return *this; }
		SamplerDesc& SetMagFilter(bool enable) { magFilter = enable; return *this; }
		SamplerDesc& SetMipFilter(bool enable) { mipFilter = enable; return *this; }
		SamplerDesc& SetAllFilters(bool enable) { minFilter = magFilter = mipFilter = enable; return *this; }
		SamplerDesc& SetAddressU(SamplerAddressMode mode) { addressU = mode; return *this; }
		SamplerDesc& SetAddressV(SamplerAddressMode mode) { addressV = mode; return *this; }
		SamplerDesc& SetAddressW(SamplerAddressMode mode) { addressW = mode; return *this; }
		SamplerDesc& SetAllAddressModes(SamplerAddressMode mode) { addressU = addressV = addressW = mode; return *this; }

	};

	class ISamplerState : public RefCounter<IResource>
	{
	public:
		SamplerDesc& GetDesc() { return desc; }
	private:
		SamplerDesc  desc;
	};
	typedef RefCountPtr<ISamplerState> SamplerStateHandle;

//**************************************************************
//**				Shader									  **
//**************************************************************
	class IUniformBuffer : public IBuffer
	{
	public:
		virtual ~IUniformBuffer() {}
	};
	typedef RefCountPtr<IUniformBuffer> UniformBufferHandle;

	enum class ShaderType : uint16_t
	{
		None = 0x0000,
		Vertex = 0x0001,
		Pixel = 0x002,
		Compute = 0x004,
	};

	struct ShaderDesc
	{
		ShaderType shaderType = ShaderType::None;
		std::string name;
		std::string entryName = "main";
	};

	class IShader : public RefCounter<IResource>
	{
	public:
		ShaderDesc& GetDesc() { return desc; };
	private:
		ShaderDesc desc;
	};

	typedef RefCountPtr<IShader> ShaderHandle;
//**************************************************************
//**				Raster									  **
//**************************************************************
	enum class PrimitiveType : uint8_t
	{
		PointList,
		LineList,
		TriangleList,
		TriangleStrip,
		TriangleFan,
		TriangleListWithAdjacency,
		TriangleStripWithAdjacency,
		PatchList
	};

	enum class CullingMode : uint8_t {
		None,
		Front,
		Back,
		FrontAndBack
	};

	enum class RasterFillMode : uint8_t
	{
		Solid,
		Wireframe
	};

	struct RasterState
	{
		CullingMode cullMode;
		RasterFillMode fillMode;
		bool depthClipEnable = false;
		bool scissorEnable = false;
		bool multisampleEnable = false;
		int depthBias = 0;
		float depthBiasClamp = 0.f;

		constexpr RasterState& SetFillMode(RasterFillMode value) { fillMode = value; return *this; }
		constexpr RasterState& SetFillSolid() { fillMode = RasterFillMode::Solid; return *this; }
		constexpr RasterState& SetFillWireframe() { fillMode = RasterFillMode::Wireframe; return *this; }
		constexpr RasterState& SetCullMode(CullingMode value) { cullMode = value; return *this; }
		constexpr RasterState& SetCullBack() { cullMode = CullingMode::Back; return *this; }
		constexpr RasterState& SetCullFront() { cullMode = CullingMode::Front; return *this; }
		constexpr RasterState& SetCullNone() { cullMode = CullingMode::None; return *this; }

		constexpr RasterState& SetDepthClipEnable(bool value) { depthClipEnable = value; return *this; }
		constexpr RasterState& EnableDepthClip() { depthClipEnable = true; return *this; }
		constexpr RasterState& DisableDepthClip() { depthClipEnable = false; return *this; }
		constexpr RasterState& SetScissorEnable(bool value) { scissorEnable = value; return *this; }
		constexpr RasterState& EnableScissor() { scissorEnable = true; return *this; }
		constexpr RasterState& DisableScissor() { scissorEnable = false; return *this; }
		constexpr RasterState& SetMultisampleEnable(bool value) { multisampleEnable = value; return *this; }
		constexpr RasterState& EnableMultisample() { multisampleEnable = true; return *this; }
		constexpr RasterState& DisableMultisample() { multisampleEnable = false; return *this; }
		constexpr RasterState& SetDepthBias(int value) { depthBias = value; return *this; }
		constexpr RasterState& SetDepthBiasClamp(float value) { depthBiasClamp = value; return *this; }
	};

//**************************************************************
//**				Depth Stencil							  **
//**************************************************************

	enum class ComparisonFunc : uint8_t
	{
		Never = 1,
		Less = 2,
		Equal = 3,
		LessOrEqual = 4,
		Greater = 5,
		NotEqual = 6,
		GreaterOrEqual = 7,
		Always = 8
	};

	enum class StencilOp : uint8_t
	{
		Keep = 1,
		Zero = 2,
		Replace = 3,
		IncrementAndClamp = 4,
		DecrementAndClamp = 5,
		Invert = 6,
		IncrementAndWrap = 7,
		DecrementAndWrap = 8
	};

	struct StencilOpDesc
	{
		StencilOp failOp = StencilOp::Keep;
		StencilOp depthFailOp = StencilOp::Keep;
		StencilOp passOp = StencilOp::Keep;
		ComparisonFunc stencilFunc = ComparisonFunc::Always;

		constexpr StencilOpDesc& SetFailOp(StencilOp value) { failOp = value; return *this; }
		constexpr StencilOpDesc& SetDepthFailOp(StencilOp value) { depthFailOp = value; return *this; }
		constexpr StencilOpDesc& SetPassOp(StencilOp value) { passOp = value; return *this; }
		constexpr StencilOpDesc& SetStencilFunc(ComparisonFunc value) { stencilFunc = value; return *this; }
	};

	struct DepthStencilState
	{
		bool            depthTestEnable = true;
		bool            depthWriteEnable = true;
		ComparisonFunc  depthFunc = ComparisonFunc::Less;
		bool            stencilEnable = false;
		uint8_t         stencilReadMask = 0xff;
		uint8_t         stencilWriteMask = 0xff;
		uint8_t         stencilRefValue = 0;
		StencilOpDesc   frontFaceStencil;
		StencilOpDesc   backFaceStencil;

		constexpr DepthStencilState& SetDepthTestEnable(bool value) { depthTestEnable = value; return *this; }
		constexpr DepthStencilState& EnableDepthTest() { depthTestEnable = true; return *this; }
		constexpr DepthStencilState& DisableDepthTest() { depthTestEnable = false; return *this; }
		constexpr DepthStencilState& SetDepthWriteEnable(bool value) { depthWriteEnable = value; return *this; }
		constexpr DepthStencilState& EnableDepthWrite() { depthWriteEnable = true; return *this; }
		constexpr DepthStencilState& DisableDepthWrite() { depthWriteEnable = false; return *this; }
		constexpr DepthStencilState& SetDepthFunc(ComparisonFunc value) { depthFunc = value; return *this; }
		constexpr DepthStencilState& SetStencilEnable(bool value) { stencilEnable = value; return *this; }
		constexpr DepthStencilState& EnableStencil() { stencilEnable = true; return *this; }
		constexpr DepthStencilState& DisableStencil() { stencilEnable = false; return *this; }
		constexpr DepthStencilState& SetStencilReadMask(uint8_t value) { stencilReadMask = value; return *this; }
		constexpr DepthStencilState& SetStencilWriteMask(uint8_t value) { stencilWriteMask = value; return *this; }
		constexpr DepthStencilState& SetStencilRefValue(uint8_t value) { stencilRefValue = value; return *this; }
		constexpr DepthStencilState& SetFrontFaceStencil(const StencilOpDesc& value) { frontFaceStencil = value; return *this; }
		constexpr DepthStencilState& SetBackFaceStencil(const StencilOpDesc& value) { backFaceStencil = value; return *this; }

	};

	enum class BlendFactor : uint8_t
	{
		Zero = 1,
		One = 2,
		SrcColor = 3,
		InvSrcColor = 4,
		SrcAlpha = 5,
		InvSrcAlpha = 6,
		DstAlpha = 7,
		InvDstAlpha = 8,
		DstColor = 9,
		InvDstColor = 10,
		SrcAlphaSaturate = 11,
		ConstantColor = 14,
		InvConstantColor = 15,
		Src1Color = 16,
		InvSrc1Color = 17,
		Src1Alpha = 18,
		InvSrc1Alpha = 19,
	};

	enum class BlendOp : uint8_t
	{
		Add = 1,
		Subrtact = 2,
		ReverseSubtract = 3,
		Min = 4,
		Max = 5
	};

	struct BlendStateDesc
	{
		bool alphaToCoverageEnable = false;
		bool        blendEnable = false;
		BlendFactor srcBlend = BlendFactor::One;
		BlendFactor destBlend = BlendFactor::Zero;
		BlendOp     blendOp = BlendOp::Add;
		BlendFactor srcBlendAlpha = BlendFactor::One;
		BlendFactor destBlendAlpha = BlendFactor::Zero;
		BlendOp     blendOpAlpha = BlendOp::Add;

		constexpr BlendStateDesc& SetAlphaToCoverageEnable(bool enable) { alphaToCoverageEnable = enable; return *this; }
		constexpr BlendStateDesc& EnableAlphaToCoverage() { alphaToCoverageEnable = true; return *this; }
		constexpr BlendStateDesc& DisableAlphaToCoverage() { alphaToCoverageEnable = false; return *this; }
		constexpr BlendStateDesc& SetBlendEnable(bool enable) { blendEnable = enable; return *this; }
		constexpr BlendStateDesc& EnableBlend() { blendEnable = true; return *this; }
		constexpr BlendStateDesc& DisableBlend() { blendEnable = false; return *this; }
		constexpr BlendStateDesc& SetSrcBlend(BlendFactor value) { srcBlend = value; return *this; }
		constexpr BlendStateDesc& SetDestBlend(BlendFactor value) { destBlend = value; return *this; }
		constexpr BlendStateDesc& SetBlendOp(BlendOp value) { blendOp = value; return *this; }
		constexpr BlendStateDesc& SetSrcBlendAlpha(BlendFactor value) { srcBlendAlpha = value; return *this; }
		constexpr BlendStateDesc& SetDestBlendAlpha(BlendFactor value) { destBlendAlpha = value; return *this; }
		constexpr BlendStateDesc& SetBlendOpAlpha(BlendOp value) { blendOpAlpha = value; return *this; }
	};


	struct RenderState
	{
		BlendStateDesc blendState;
		DepthStencilState depthStencilState;
		RasterState rasterState;

		constexpr RenderState& SetBlendState(const BlendStateDesc& value) { blendState = value; return *this; }
		constexpr RenderState& SetDepthStencilState(const DepthStencilState& value) { depthStencilState = value; return *this; }
		constexpr RenderState& SetRasterState(const RasterState& value) { rasterState = value; return *this; }
	};

	class IFrameBuffer : public RefCounter<IResource>
	{
	public:
		std::vector<TextureHandle> color;
		TextureHandle depth;
	};
	typedef RefCountPtr<IFrameBuffer> FrameBufferHandle;

	struct PipelineDesc
	{
		PrimitiveType primType = PrimitiveType::TriangleList;
		RenderState renderState;

		ShaderHandle VS;
		ShaderHandle FS;

		PipelineDesc& SetPrimType(PrimitiveType value) { primType = value; return *this; }
		PipelineDesc& SetVertexShader(IShader* value) { VS = value; return *this; }
		PipelineDesc& SetFragmentShader(IShader* value) { FS = value; return *this; }
	};

	class IPipelineState : public RefCounter<IResource>
	{
	public:
		PipelineDesc& GetDesc() { return desc; };
	private:
		PipelineDesc desc;
	};
	typedef RefCountPtr<IPipelineState> PipelineStateHandle;

	struct ComputePipelineDesc
	{
		ShaderHandle CS;
		ComputePipelineDesc& SetComputeShader(IShader* value) { CS = value; return *this; }
	};

	class IComputePipelineState : public RefCounter<IResource>
	{
	public:
		ComputePipelineDesc& GetDesc() { return desc; };
	private:
		ComputePipelineDesc desc;
	};
	typedef RefCountPtr<IComputePipelineState> ComputePipelineState;
}
}