#pragma once
#include <string>
#include <stdio.h>
#include "ostream.h"
#ifndef LOGD
#define LOGD(message) redtea::common::rlog.d<<##message##<<redtea::common::endl
#endif // !LOGD

#ifndef LOGE
#define LOGE(message) redtea::common::rlog.e<<##message##<<redtea::common::endl
#endif // !LOGE

#ifndef LOGI
#define LOGI(message) redtea::common::rlog.i<<##message##<<redtea::common::endl
#endif // !LOGI

#ifndef LOGW
#define LOGW(message) redtea::common::rlog.w<<##message##<<redtea::common::endl
#endif // !LOGW
namespace redtea
{
namespace common
{
	class LoggerStream : public ostream
	{
	public:
		enum Priority
		{
			LOG_DEBUG, LOG_ERROR, LOG_WARNING, LOG_INFO
		};

		explicit LoggerStream(Priority p) noexcept;

		ostream& flush() noexcept override;

		void SetOutputFile(std::string file);
	private:
		Priority mPriority;
		FILE* info;
		FILE* error;
	};

	struct Logger
	{
		LoggerStream& d;
		LoggerStream& e;
		LoggerStream& w;
		LoggerStream& i;
	};

	extern Logger const rlog;
}
}