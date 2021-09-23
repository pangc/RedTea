#pragma once
#include "../../RHI/rhi.h"
#include "GLES3/gl3.h"
#include <array>

namespace redtea
{
namespace device
{
	class GLSwapChain : public ISwapChain
	{
	public:
		GLSwapChain() = default;
		void Init(void* native);
		void* GetNativeResource() { return nativeResource; };
		virtual void Resize(uint16_t width, uint16_t height) override;
	private:
		void* nativeResource;
	};

	class GLBuffer : public IBuffer
	{
	public:
		GLBuffer(BufferDesc d);
		GLuint id;
		GLenum target;
		GLenum usage;
	};

	class GLRenderPrimitive : public IRenderPrimitive
	{
	public:
		GLuint vao = 0;
		std::array<GLuint, MAX_VERTEX_ATTRIBUTE_COUNT> buffers;
	};
}
}