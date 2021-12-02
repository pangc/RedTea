#pragma once

#include "../../RHI/rhi.h"
#include <dxgi.h>

namespace redtea
{
namespace device
{
	struct DxgiFormatMapping
	{
		Format abstractFormat;
		DXGI_FORMAT resourceFormat;
		DXGI_FORMAT srvFormat;
		DXGI_FORMAT rtvFormat;
	};

	const DxgiFormatMapping& getDxgiFormatMapping(Format abstractFormat);
}
}