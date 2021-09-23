#include "gl_context.h"
#include "gl_context.h"

namespace redtea{
namespace device {

void GLContext::BindBuffer(GLenum target, GLint id)
{
	size_t index = getIndexForBufferTarget(target);
	if (bufferBindings[index] != id)
	{
		glBindBuffer(target, id);
	}
}

constexpr size_t GLContext::getIndexForBufferTarget(GLenum target) noexcept
{
	size_t index = 0;
	switch (target) {
	case GL_UNIFORM_BUFFER:             index = 0; break;
	case GL_TRANSFORM_FEEDBACK_BUFFER:  index = 1; break;

	case GL_ARRAY_BUFFER:               index = 2; break;
	case GL_COPY_READ_BUFFER:           index = 3; break;
	case GL_COPY_WRITE_BUFFER:          index = 4; break;
	case GL_ELEMENT_ARRAY_BUFFER:       index = 5; break;
	case GL_PIXEL_PACK_BUFFER:          index = 6; break;
	case GL_PIXEL_UNPACK_BUFFER:        index = 7; break;
	default: index = 8; break;
	}
	return index;
}

}
}