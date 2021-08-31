#pragma once
#include "iresource.h"

namespace redtea
{
namespace device
{
	class ISwapChain : public IResource
	{
	public:
		void Resize(uint16_t width, uint16_t height);
	};
}
}