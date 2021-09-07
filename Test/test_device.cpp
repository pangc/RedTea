#include "../Engine/Runtime/Device/RHI/resource.h"
#include "../Engine/Runtime/Device/RHI/command_buffer.h"
#include "common.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>


TEST(RESOURCE_TEST, resource)
{
	using namespace redtea::device;
	class ITexture : public IResource
	{

	};

	class Texture : public RefCounter<IResource>
	{
		~Texture()
		{
			std::cout << "release" << std::endl;
		}
	};

	typedef RefCountPtr<Texture> TextureHandler;
	Texture* t = new Texture();
	// attach will not incref
	TextureHandler handler = TextureHandler::Create(t);
	EXPECT_EQ(t->GetCount(), 1);
	// incref
	TextureHandler handlerb = handler;
	EXPECT_EQ(t->GetCount(), 2);
	// decref
	handlerb = nullptr;
	// decref
	handler = nullptr;
	// call release
	std::cout << "have release" << std::endl;
}

#if USE_MULTTRHEAD
TEST(RHI_TEST, writebuffer_multitread)
{
	using namespace redtea::device;
	CommandBuffer buffer(10);
	static int count = 0;
	std::thread writer([&]()
	{
		for (int i = 0; i < 10000; i++)
		{
			buffer.WriteCommand<CustomCommand>([i]()
			{
				//std::cout << "reading " << i << " command" << std::endl;
				count++;
			});

			//std::cout << "writing " << i << " command" << std::endl;
			if (i % 100 == 0)
			{
				buffer.KickOff();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		buffer.RequestExit();
	});

	std::thread reader([&]()
	{
		buffer.WaitAndFlush();
	});

	writer.join();
	reader.join();
	
	EXPECT_EQ(count, 10000);
}
#else
TEST(RHI_TEST, writebuffer_singletread)
{
	using namespace redtea::device;
	CommandBuffer buffer(10);
	static int count = 0;
	for (int i = 0; i < 10000; i++)
	{
		buffer.WriteCommand<CustomCommand>([i]()
		{
			count++;
		});
	}
	buffer.Flush();
	EXPECT_EQ(count, 10000);
}
#endif