#include "rhi_utils.h"
#include <sstream>

namespace redtea {
namespace device {
    namespace utils {

    const char* GraphicsAPIToString(GraphicsAPI api)
    {
        switch (api)
        {
        case GraphicsAPI::D3D11:  return "D3D11";
        case GraphicsAPI::D3D12:  return "D3D12";
        case GraphicsAPI::VULKAN: return "Vulkan";
        default:                         return "<UNKNOWN>";
        }
    }

    const char* TextureDimensionToString(TextureDimension dimension)
    {
        switch (dimension)
        {
        case TextureDimension::Texture1D:           return "Texture1D";
        case TextureDimension::Texture1DArray:      return "Texture1DArray";
        case TextureDimension::Texture2D:           return "Texture2D";
        case TextureDimension::Texture2DArray:      return "Texture2DArray";
        case TextureDimension::TextureCube:         return "TextureCube";
        case TextureDimension::TextureCubeArray:    return "TextureCubeArray";
        case TextureDimension::Texture2DMS:         return "Texture2DMS";
        case TextureDimension::Texture2DMSArray:    return "Texture2DMSArray";
        case TextureDimension::Texture3D:           return "Texture3D";
        case TextureDimension::Unknown:             return "Unknown";
        default:                                    return "<INVALID>";
        }
    }

    const char* DebugNameToString(const std::string& debugName)
    {
        return debugName.empty() ? "<UNNAMED>" : debugName.c_str();
    }


    const char* ShaderStageToString(ShaderType stage)
    {
        switch (stage)
        {
        case ShaderType::None:          return "None";
        case ShaderType::Compute:       return "Compute";
        case ShaderType::Vertex:        return "Vertex";
        case ShaderType::Hull:          return "Hull";
        case ShaderType::Domain:        return "Domain";
        case ShaderType::Geometry:      return "Geometry";
        case ShaderType::Pixel:         return "Pixel";
        case ShaderType::Amplification: return "Amplification";
        case ShaderType::Mesh:          return "Mesh";
        case ShaderType::AllGraphics:   return "AllGraphics";
        case ShaderType::RayGeneration: return "RayGeneration";
        case ShaderType::AnyHit:        return "AnyHit";
        case ShaderType::ClosestHit:    return "ClosestHit";
        case ShaderType::Miss:          return "Miss";
        case ShaderType::Intersection:  return "Intersection";
        case ShaderType::Callable:      return "Callable";
        case ShaderType::AllRayTracing: return "AllRayTracing";
        case ShaderType::All:           return "All";
        default:                        return "<INVALID>";
        }
    }

    const char* ResourceTypeToString(ResourceType type)
    {
        switch (type)
        {
        case ResourceType::None:                    return "None";
        case ResourceType::Texture_SRV:             return "Texture_SRV";
        case ResourceType::Texture_UAV:             return "Texture_UAV";
        case ResourceType::TypedBuffer_SRV:         return "Buffer_SRV";
        case ResourceType::TypedBuffer_UAV:         return "Buffer_UAV";
        case ResourceType::StructuredBuffer_SRV:    return "StructuredBuffer_SRV";
        case ResourceType::StructuredBuffer_UAV:    return "StructuredBuffer_UAV";
        case ResourceType::RawBuffer_SRV:           return "RawBuffer_SRV";
        case ResourceType::RawBuffer_UAV:           return "RawBuffer_UAV";
        case ResourceType::ConstantBuffer:          return "ConstantBuffer";
        case ResourceType::VolatileConstantBuffer:  return "VolatileConstantBuffer";
        case ResourceType::Sampler:                 return "Sampler";
        case ResourceType::RayTracingAccelStruct:   return "RayTracingAccelStruct";
        case ResourceType::PushConstants:           return "PushConstants";
        case ResourceType::Count:
        default:                                    return "<INVALID>";
        }
    }

    const char* FormatToString(Format format)
    {
        return getFormatInfo(format).name;
    }

    const char* CommandQueueToString(CommandQueue queue)
    {
        switch(queue)
        {
        case CommandQueue::Graphics: return "Graphics";
        case CommandQueue::Compute:  return "Compute";
        case CommandQueue::Copy:     return "Copy";
        case CommandQueue::Count:
        default:
            return "<INVALID>";
        }
    }

    std::string GenerateHeapDebugName(const HeapDesc& desc)
    {
        std::stringstream ss;

        ss << "Unnamed ";

        switch(desc.type)
        {
        case HeapType::DeviceLocal:
            ss << "DeviceLocal";
            break;
        case HeapType::Upload: 
            ss << "Upload";
            break;
        case HeapType::Readback: 
            ss << "Readback";
            break;
        default: 
            ss << "Invalid-Type";
            break;
        }

        ss << " Heap (" << desc.capacity << " bytes)";

        return ss.str();
    }

    std::string GenerateTextureDebugName(const TextureDesc& desc)
    {
        std::stringstream ss;

        ss << "Unnamed " << TextureDimensionToString(desc.dimension);
        ss << " (" << getFormatInfo(desc.format).name;
        ss << ", Width = " << desc.width;

        if (desc.dimension >= TextureDimension::Texture2D)
            ss << ", Height = " << desc.height;

        if (desc.dimension == TextureDimension::Texture3D)
            ss << ", Depth = " << desc.depth;

        if (desc.dimension == TextureDimension::Texture1DArray ||
            desc.dimension == TextureDimension::Texture2DArray ||
            desc.dimension == TextureDimension::TextureCubeArray ||
            desc.dimension == TextureDimension::Texture2DMSArray)
            ss << ", ArraySize = " << desc.arraySize;

        if (desc.dimension == TextureDimension::Texture1D ||
            desc.dimension == TextureDimension::Texture1DArray ||
            desc.dimension == TextureDimension::Texture2D ||
            desc.dimension == TextureDimension::Texture2DArray ||
            desc.dimension == TextureDimension::TextureCube ||
            desc.dimension == TextureDimension::TextureCubeArray)
            ss << ", MipLevels = " << desc.mipLevels;

        if (desc.dimension == TextureDimension::Texture2DMS ||
            desc.dimension == TextureDimension::Texture2DMSArray)
            ss << ", SampleCount = " << desc.sampleCount << ", SampleQuality = " << desc.sampleQuality;

        if (desc.isRenderTarget) ss << ", IsRenderTarget";
        if (desc.isUAV)          ss << ", IsUAV";
        if (desc.isTypeless)     ss << ", IsTypeless";

        ss << ")";

        return ss.str();
    }

    std::string GenerateBufferDebugName(const BufferDesc& desc)
    {
        std::stringstream ss;

        ss << "Unnamed Buffer (ByteSize = " << desc.byteSize;

        if (desc.format != Format::UNKNOWN)
            ss << ", Format = " << getFormatInfo(desc.format).name;

        if (desc.structStride > 0)
            ss << ", StructStride = " << desc.structStride;

        if (desc.isVolatile)
            ss << ", IsVolatile, MaxVersions = " << desc.maxVersions;

        if (desc.canHaveUAVs) ss << ", CanHaveUAVs";
        if (desc.canHaveTypedViews) ss << ", CanHaveTypedViews";
        if (desc.canHaveRawViews) ss << ", CanHaveRawViews";
        if (desc.isVertexBuffer) ss << ", IsVertexBuffer";
        if (desc.isIndexBuffer) ss << ", IsIndexBuffer";
        if (desc.isConstantBuffer) ss << ", IsConstantBuffer";
        if (desc.isDrawIndirectArgs) ss << ", IsDrawIndirectArgs";
        if (desc.isAccelStructBuildInput) ss << ", IsAccelStructBuildInput";
        if (desc.isAccelStructStorage) ss << ", IsAccelStructStorage";

        ss << ")";

        return ss.str();
    }

    void NotImplemented()
    {
        assert(!"Not Implemented");  // NOLINT(clang-diagnostic-string-conversion)
    }

    void NotSupported()
    {
        assert(!"Not Supported");  // NOLINT(clang-diagnostic-string-conversion)
    }

    void InvalidEnum()
    {
        assert(!"Invalid Enumeration Value");  // NOLINT(clang-diagnostic-string-conversion)
    }

    }
}
}