#include "gl_resource.h"

namespace redtea
{
namespace device
{
	GLSwapChain::GLSwapChain(void * native)
	{
		nativeResource = native;
	}

	void GLSwapChain::Resize(uint16_t width, uint16_t height)
	{
	}

}
}