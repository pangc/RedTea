#include "gl_resource.h"
#include "common.h"

namespace redtea
{
namespace device
{

void GLSwapChain::Init(void * native)
{
	nativeResource = native;
}

void GLSwapChain::Resize(uint16_t width, uint16_t height)
{
}

GLBuffer::GLBuffer(const BufferDesc& d) : IBuffer(d)
{
	switch (desc.type)
	{
	case BufferType::VertexBuffer:
		target = GL_ARRAY_BUFFER;
		usage = GL_STATIC_DRAW;
		break;
	case BufferType::IndexBuffer:
		target = GL_ELEMENT_ARRAY_BUFFER;
		usage = GL_STATIC_DRAW;
		break;
	case BufferType::UniformBuffer:
		target = GL_UNIFORM_BUFFER;
		usage = GL_DYNAMIC_DRAW;
		break;
	}
}

}
}