#include "rhi.h"

namespace redtea {
namespace device {
    constexpr uint64_t c_VersionSubmittedFlag = 0x8000000000000000;
    constexpr uint32_t c_VersionQueueShift = 60;
    constexpr uint32_t c_VersionQueueMask = 0x7;
    constexpr uint64_t c_VersionIDMask = 0x0FFFFFFFFFFFFFFF;

    constexpr uint64_t MakeVersion(uint64_t id, CommandQueue queue, bool submitted)
    {
        uint64_t result = (id & c_VersionIDMask) | (uint64_t(queue) << c_VersionQueueShift);
        if (submitted) result |= c_VersionSubmittedFlag;
        return result;
    }

    constexpr uint64_t VersionGetInstance(uint64_t version)
    {
        return version & c_VersionIDMask;
    }

    constexpr CommandQueue VersionGetQueue(uint64_t version)
    {
        return CommandQueue((version >> c_VersionQueueShift) & c_VersionQueueMask);
    }

    constexpr bool VersionGetSubmitted(uint64_t version)
    {
        return (version & c_VersionSubmittedFlag) != 0;
    }

namespace utils {
    const char* GraphicsAPIToString(GraphicsAPI api);
    const char* TextureDimensionToString(TextureDimension dimension);
    const char* DebugNameToString(const std::string& debugName);
    const char* ShaderStageToString(ShaderType stage);
    const char* ResourceTypeToString(ResourceType type);
    const char* FormatToString(Format format);
    const char* CommandQueueToString(CommandQueue queue);

    std::string GenerateHeapDebugName(const HeapDesc& desc);
    std::string GenerateTextureDebugName(const TextureDesc& desc);
    std::string GenerateBufferDebugName(const BufferDesc& desc);

    void NotImplemented();
    void NotSupported();
    void InvalidEnum();

}
}
}