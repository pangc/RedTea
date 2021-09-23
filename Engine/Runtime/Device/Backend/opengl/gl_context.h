#pragma once
#include "GLES3/gl3.h"

namespace redtea
{
namespace device
{
	class GLContext
	{
	public:
		void BindBuffer(GLenum target, GLint id);

		constexpr size_t getIndexForBufferTarget(GLenum target) noexcept;

		// ���浱ǰ�󶨵�buffer״̬
		GLint bufferBindings[8] = {0};
	};
}
}