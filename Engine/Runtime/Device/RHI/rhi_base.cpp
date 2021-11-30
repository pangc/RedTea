#include "rhi.h"
#include <sstream>

namespace redtea {
namespace device {
    
    TextureSlice TextureSlice::resolve(const TextureDesc& desc) const
    {
        TextureSlice ret(*this);

        assert(mipLevel < desc.mipLevels);

        if (width == uint32_t(-1))
            ret.width = (desc.width >> mipLevel);

        if (height == uint32_t(-1))
            ret.height = (desc.height >> mipLevel);

        if (depth == uint32_t(-1))
        {
            if (desc.dimension == TextureDimension::Texture3D)
                ret.depth = (desc.depth >> mipLevel);
            else
                ret.depth = 1;
        }

        return ret;
    }
        
    TextureSubresourceSet TextureSubresourceSet::resolve(const TextureDesc& desc, bool singleMipLevel) const
    {
        TextureSubresourceSet ret;
        ret.baseMipLevel = baseMipLevel;

        if (singleMipLevel)
        {
            ret.numMipLevels = 1;
        }
        else
        {
            int lastMipLevelPlusOne = std::min(baseMipLevel + numMipLevels, desc.mipLevels);
            ret.numMipLevels = MipLevel(std::max(0u, lastMipLevelPlusOne - baseMipLevel));
        }

        switch (desc.dimension)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case TextureDimension::Texture1DArray:
        case TextureDimension::Texture2DArray:
        case TextureDimension::TextureCube:
        case TextureDimension::TextureCubeArray:
        case TextureDimension::Texture2DMSArray: {
            ret.baseArraySlice = baseArraySlice;
            int lastArraySlicePlusOne = std::min(baseArraySlice + numArraySlices, desc.arraySize);
            ret.numArraySlices = ArraySlice(std::max(0u, lastArraySlicePlusOne - baseArraySlice));
            break;
        }
        default: 
            ret.baseArraySlice = 0;
            ret.numArraySlices = 1;
            break;
        }

        return ret;
    }

    bool TextureSubresourceSet::isEntireTexture(const TextureDesc& desc) const
    {
        if (baseMipLevel > 0u || baseMipLevel + numMipLevels < desc.mipLevels)
            return false;

        switch (desc.dimension)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case TextureDimension::Texture1DArray:
        case TextureDimension::Texture2DArray:
        case TextureDimension::TextureCube:
        case TextureDimension::TextureCubeArray:
        case TextureDimension::Texture2DMSArray: 
            if (baseArraySlice > 0u || baseArraySlice + numArraySlices < desc.arraySize)
                return false;
        default:
            return true;
        }
    }
    
    BufferRange BufferRange::resolve(const BufferDesc& desc) const
    {
        BufferRange result;
        result.byteOffset = std::min(byteOffset, desc.byteSize);
        if (byteSize == 0)
            result.byteSize = desc.byteSize - result.byteOffset;
        else
            result.byteSize = std::min(byteSize, desc.byteSize - result.byteOffset);
        return result;
    }
    
    bool BlendState::RenderTarget::usesConstantColor() const
    {
        return srcBlend == BlendFactor::ConstantColor || srcBlend == BlendFactor::OneMinusConstantColor ||
            destBlend == BlendFactor::ConstantColor || destBlend == BlendFactor::OneMinusConstantColor ||
            srcBlendAlpha == BlendFactor::ConstantColor || srcBlendAlpha == BlendFactor::OneMinusConstantColor ||
            destBlendAlpha == BlendFactor::ConstantColor || destBlendAlpha == BlendFactor::OneMinusConstantColor;
    }

    bool BlendState::usesConstantColor(uint32_t numTargets) const
    {
        for (uint32_t rt = 0; rt < numTargets; rt++)
        {
            if (targets[rt].usesConstantColor())
                return true;
        }

        return false;
    }
    
    FramebufferInfo::FramebufferInfo(const FramebufferDesc& desc)
    {
        for (size_t i = 0; i < desc.colorAttachments.size(); i++)
        {
            const FramebufferAttachment& attachment = desc.colorAttachments[i];
            colorFormats.push_back(attachment.format == Format::UNKNOWN && attachment.texture ? attachment.texture->getDesc().format : attachment.format);
        }

        if (desc.depthAttachment.valid())
        {
            const TextureDesc& textureDesc = desc.depthAttachment.texture->getDesc();
            depthFormat = textureDesc.format;
            width = textureDesc.width >> desc.depthAttachment.subresources.baseMipLevel;
            height = textureDesc.height >> desc.depthAttachment.subresources.baseMipLevel;
            sampleCount = textureDesc.sampleCount;
            sampleQuality = textureDesc.sampleQuality;
        }
        else if (!desc.colorAttachments.empty() && desc.colorAttachments[0].valid())
        {
            const TextureDesc& textureDesc = desc.colorAttachments[0].texture->getDesc();
            width = textureDesc.width >> desc.colorAttachments[0].subresources.baseMipLevel;
            height = textureDesc.height >> desc.colorAttachments[0].subresources.baseMipLevel;
            sampleCount = textureDesc.sampleCount;
            sampleQuality = textureDesc.sampleQuality;
        }
    }
    
    void ICommandList::setResourceStatesForFramebuffer(IFramebuffer* framebuffer, bool enableDepthWrite)
    {
        const FramebufferDesc& desc = framebuffer->getDesc();

        for (const auto& attachment : desc.colorAttachments)
        {
            setTextureState(attachment.texture, attachment.subresources,
                ResourceStates::RenderTarget);
        }

        if (desc.depthAttachment.valid())
        {
            setTextureState(desc.depthAttachment.texture, desc.depthAttachment.subresources,
                enableDepthWrite ? ResourceStates::DepthWrite : ResourceStates::DepthRead);
        }
    }

    // Format mapping table. The rows must be in the exactly same order as Format enum members are defined.
    static const FormatInfo c_FormatInfo[] = {
    //        format                   name             bytes blk         kind               red   green   blue  alpha  depth  stencl signed  srgb
        { Format::UNKNOWN,           "UNKNOWN",           0,   0, FormatKind::Integer,      false, false, false, false, false, false, false, false },
        { Format::R8_UINT,           "R8_UINT",           1,   1, FormatKind::Integer,      true,  false, false, false, false, false, false, false },
        { Format::R8_SINT,           "R8_SINT",           1,   1, FormatKind::Integer,      true,  false, false, false, false, false, true,  false },
        { Format::R8_UNORM,          "R8_UNORM",          1,   1, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { Format::R8_SNORM,          "R8_SNORM",          1,   1, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { Format::RG8_UINT,          "RG8_UINT",          2,   1, FormatKind::Integer,      true,  true,  false, false, false, false, false, false },
        { Format::RG8_SINT,          "RG8_SINT",          2,   1, FormatKind::Integer,      true,  true,  false, false, false, false, true,  false },
        { Format::RG8_UNORM,         "RG8_UNORM",         2,   1, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { Format::RG8_SNORM,         "RG8_SNORM",         2,   1, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { Format::R16_UINT,          "R16_UINT",          2,   1, FormatKind::Integer,      true,  false, false, false, false, false, false, false },
        { Format::R16_SINT,          "R16_SINT",          2,   1, FormatKind::Integer,      true,  false, false, false, false, false, true,  false },
        { Format::R16_UNORM,         "R16_UNORM",         2,   1, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { Format::R16_SNORM,         "R16_SNORM",         2,   1, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { Format::R16_FLOAT,         "R16_FLOAT",         2,   1, FormatKind::Float,        true,  false, false, false, false, false, true,  false },
        { Format::BGRA4_UNORM,       "BGRA4_UNORM",       2,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::B5G6R5_UNORM,      "B5G6R5_UNORM",      2,   1, FormatKind::Normalized,   true,  true,  true,  false, false, false, false, false },
        { Format::B5G5R5A1_UNORM,    "B5G5R5A1_UNORM",    2,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::RGBA8_UINT,        "RGBA8_UINT",        4,   1, FormatKind::Integer,      true,  true,  true,  true,  false, false, false, false },
        { Format::RGBA8_SINT,        "RGBA8_SINT",        4,   1, FormatKind::Integer,      true,  true,  true,  true,  false, false, true,  false },
        { Format::RGBA8_UNORM,       "RGBA8_UNORM",       4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::RGBA8_SNORM,       "RGBA8_SNORM",       4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::BGRA8_UNORM,       "BGRA8_UNORM",       4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::SRGBA8_UNORM,      "SRGBA8_UNORM",      4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { Format::SBGRA8_UNORM,      "SBGRA8_UNORM",      4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::R10G10B10A2_UNORM, "R10G10B10A2_UNORM", 4,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::R11G11B10_FLOAT,   "R11G11B10_FLOAT",   4,   1, FormatKind::Float,        true,  true,  true,  false, false, false, false, false },
        { Format::RG16_UINT,         "RG16_UINT",         4,   1, FormatKind::Integer,      true,  true,  false, false, false, false, false, false },
        { Format::RG16_SINT,         "RG16_SINT",         4,   1, FormatKind::Integer,      true,  true,  false, false, false, false, true,  false },
        { Format::RG16_UNORM,        "RG16_UNORM",        4,   1, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { Format::RG16_SNORM,        "RG16_SNORM",        4,   1, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { Format::RG16_FLOAT,        "RG16_FLOAT",        4,   1, FormatKind::Float,        true,  true,  false, false, false, false, true,  false },
        { Format::R32_UINT,          "R32_UINT",          4,   1, FormatKind::Integer,      true,  false, false, false, false, false, false, false },
        { Format::R32_SINT,          "R32_SINT",          4,   1, FormatKind::Integer,      true,  false, false, false, false, false, true,  false },
        { Format::R32_FLOAT,         "R32_FLOAT",         4,   1, FormatKind::Float,        true,  false, false, false, false, false, true,  false },
        { Format::RGBA16_UINT,       "RGBA16_UINT",       8,   1, FormatKind::Integer,      true,  true,  true,  true,  false, false, false, false },
        { Format::RGBA16_SINT,       "RGBA16_SINT",       8,   1, FormatKind::Integer,      true,  true,  true,  true,  false, false, true,  false },
        { Format::RGBA16_FLOAT,      "RGBA16_FLOAT",      8,   1, FormatKind::Float,        true,  true,  true,  true,  false, false, true,  false },
        { Format::RGBA16_UNORM,      "RGBA16_UNORM",      8,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::RGBA16_SNORM,      "RGBA16_SNORM",      8,   1, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::RG32_UINT,         "RG32_UINT",         8,   1, FormatKind::Integer,      true,  true,  false, false, false, false, false, false },
        { Format::RG32_SINT,         "RG32_SINT",         8,   1, FormatKind::Integer,      true,  true,  false, false, false, false, true,  false },
        { Format::RG32_FLOAT,        "RG32_FLOAT",        8,   1, FormatKind::Float,        true,  true,  false, false, false, false, true,  false },
        { Format::RGB32_UINT,        "RGB32_UINT",        12,  1, FormatKind::Integer,      true,  true,  true,  false, false, false, false, false },
        { Format::RGB32_SINT,        "RGB32_SINT",        12,  1, FormatKind::Integer,      true,  true,  true,  false, false, false, true,  false },
        { Format::RGB32_FLOAT,       "RGB32_FLOAT",       12,  1, FormatKind::Float,        true,  true,  true,  false, false, false, true,  false },
        { Format::RGBA32_UINT,       "RGBA32_UINT",       16,  1, FormatKind::Integer,      true,  true,  true,  true,  false, false, false, false },
        { Format::RGBA32_SINT,       "RGBA32_SINT",       16,  1, FormatKind::Integer,      true,  true,  true,  true,  false, false, true,  false },
        { Format::RGBA32_FLOAT,      "RGBA32_FLOAT",      16,  1, FormatKind::Float,        true,  true,  true,  true,  false, false, true,  false },
        { Format::D16,               "D16",               2,   1, FormatKind::DepthStencil, false, false, false, false, true,  false, false, false },
        { Format::D24S8,             "D24S8",             4,   1, FormatKind::DepthStencil, false, false, false, false, true,  true,  false, false },
        { Format::X24G8_UINT,        "X24G8_UINT",        4,   1, FormatKind::Integer,      false, false, false, false, false, true,  false, false },
        { Format::D32,               "D32",               4,   1, FormatKind::DepthStencil, false, false, false, false, true,  false, false, false },
        { Format::D32S8,             "D32S8",             8,   1, FormatKind::DepthStencil, false, false, false, false, true,  true,  false, false },
        { Format::X32G8_UINT,        "X32G8_UINT",        8,   1, FormatKind::Integer,      false, false, false, false, false, true,  false, false },
        { Format::BC1_UNORM,         "BC1_UNORM",         8,   4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::BC1_UNORM_SRGB,    "BC1_UNORM_SRGB",    8,   4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { Format::BC2_UNORM,         "BC2_UNORM",         16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::BC2_UNORM_SRGB,    "BC2_UNORM_SRGB",    16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { Format::BC3_UNORM,         "BC3_UNORM",         16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::BC3_UNORM_SRGB,    "BC3_UNORM_SRGB",    16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
        { Format::BC4_UNORM,         "BC4_UNORM",         8,   4, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { Format::BC4_SNORM,         "BC4_SNORM",         8,   4, FormatKind::Normalized,   true,  false, false, false, false, false, false, false },
        { Format::BC5_UNORM,         "BC5_UNORM",         16,  4, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { Format::BC5_SNORM,         "BC5_SNORM",         16,  4, FormatKind::Normalized,   true,  true,  false, false, false, false, false, false },
        { Format::BC6H_UFLOAT,       "BC6H_UFLOAT",       16,  4, FormatKind::Float,        true,  true,  true,  false, false, false, false, false },
        { Format::BC6H_SFLOAT,       "BC6H_SFLOAT",       16,  4, FormatKind::Float,        true,  true,  true,  false, false, false, true,  false },
        { Format::BC7_UNORM,         "BC7_UNORM",         16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, false },
        { Format::BC7_UNORM_SRGB,    "BC7_UNORM_SRGB",    16,  4, FormatKind::Normalized,   true,  true,  true,  true,  false, false, false, true  },
    };

    const FormatInfo& getFormatInfo(Format format)
    {
        static_assert(sizeof(c_FormatInfo) / sizeof(FormatInfo) == size_t(Format::COUNT), 
            "The format info table doesn't have the right number of elements");

        if (uint32_t(format) >= uint32_t(Format::COUNT))
            return c_FormatInfo[0]; // UNKNOWN

        const FormatInfo& info = c_FormatInfo[uint32_t(format)];
        assert(info.format == format);
        return info;
    }

}
}