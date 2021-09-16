#pragma once
#include "../../RHI/rhi.h"

namespace redtea
{
namespace device
{
	class GLSwapChain : public ISwapChain
	{
	public:
		explicit GLSwapChain(void* native);
		void* GetNativeResource() { return nativeResource; };
		virtual void Resize(uint16_t width, uint16_t height) override;
	private:
		void* nativeResource;
	};
}
}