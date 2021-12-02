#include "d3d12-backend.h"

namespace redtea
{
namespace device
{
    ShaderHandle Device::createShader(const ShaderDesc & d, const void * binary, const size_t binarySize)
    {
        if (binarySize == 0)
            return nullptr;

        Shader* shader = new Shader();
        shader->bytecode.resize(binarySize);
        shader->desc = d;
        memcpy(&shader->bytecode[0], binary, binarySize);

        if (d.numCustomSemantics || d.pCoordinateSwizzling || (d.fastGSFlags != 0) || d.hlslExtensionsUAV >= 0)
        {
            utils::NotSupported();
            delete shader;

            // NVAPI is unavailable
            return nullptr;
        }
        
        return ShaderHandle::Create(shader);
    }
    
    ShaderHandle Device::createShaderSpecialization(IShader*, const ShaderSpecialization*, uint32_t)
    {
        utils::NotSupported();
        return nullptr;
    }

    ShaderLibraryHandle Device::createShaderLibrary(const void* binary, const size_t binarySize)
    {
        ShaderLibrary* shaderLibrary = new ShaderLibrary();

        shaderLibrary->bytecode.resize(binarySize);
        memcpy(&shaderLibrary->bytecode[0], binary, binarySize);

        return ShaderLibraryHandle::Create(shaderLibrary);
    }
    
    InputLayoutHandle Device::createInputLayout(const VertexAttributeDesc * d, uint32_t attributeCount, IShader* vertexShader)
    {
        // The shader is not needed here, there are no separate IL objects in DX12
        (void)vertexShader;

        InputLayout* layout = new InputLayout();
        layout->attributes.resize(attributeCount);

        for (uint32_t index = 0; index < attributeCount; index++)
        {
            VertexAttributeDesc& attr = layout->attributes[index];

            // Copy the description to get a stable name pointer in desc
            attr = d[index];

            assert(attr.arraySize > 0);

            const DxgiFormatMapping& formatMapping = getDxgiFormatMapping(attr.format);
            const FormatInfo& formatInfo = getFormatInfo(attr.format);

            for (uint32_t semanticIndex = 0; semanticIndex < attr.arraySize; semanticIndex++)
            {
                D3D12_INPUT_ELEMENT_DESC desc;

                desc.SemanticName = attr.name.c_str();
                desc.AlignedByteOffset = attr.offset + semanticIndex * formatInfo.bytesPerBlock;
                desc.Format = formatMapping.srvFormat;
                desc.InputSlot = attr.bufferIndex;
                desc.SemanticIndex = semanticIndex;

                if (attr.isInstanced)
                {
                    desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
                    desc.InstanceDataStepRate = 1;
                }
                else
                {
                    desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                    desc.InstanceDataStepRate = 0;
                }

                layout->inputElements.push_back(desc);
            }

            if (layout->elementStrides.find(attr.bufferIndex) == layout->elementStrides.end())
            {
                layout->elementStrides[attr.bufferIndex] = attr.elementStride;
            } else {
                assert(layout->elementStrides[attr.bufferIndex] == attr.elementStride);
            }
        }

        return InputLayoutHandle::Create(layout);
    }

    uint32_t InputLayout::getNumAttributes() const
    {
        return uint32_t(attributes.size());
    }

    const VertexAttributeDesc* InputLayout::getAttributeDesc(uint32_t index) const
    {
        if (index < uint32_t(attributes.size())) return &attributes[index];
        else return nullptr;
    }

    void Shader::getBytecode(const void** ppBytecode, size_t* pSize) const
    {
        if (ppBytecode) *ppBytecode = bytecode.data();
        if (pSize) *pSize = bytecode.size();
    }

    void ShaderLibraryEntry::getBytecode(const void** ppBytecode, size_t* pSize) const
    {
        library->getBytecode(ppBytecode, pSize);
    }

    void ShaderLibrary::getBytecode(const void** ppBytecode, size_t* pSize) const
    {
        if (ppBytecode) *ppBytecode = bytecode.data();
        if (pSize) *pSize = bytecode.size();
    }

    ShaderHandle ShaderLibrary::getShader(const char* entryName, ShaderType shaderType)
    {
        return ShaderHandle::Create(new ShaderLibraryEntry(this, entryName, shaderType));
    }
}
}