#pragma once

namespace redtea
{
namespace common
{
	class Object
	{
		Object();
		Object(const Object &) = delete;
		Object & operator = (const Object &) = delete;
		virtual ~Object();
	};

}
}