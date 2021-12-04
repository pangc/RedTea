#pragma once
#include <string>
#include <stdio.h>
#include "ostream.h"
#ifndef LOGD
#define LOGD(message) redtea::common::rlog.d<<##message##<<redtea::common::endl
#endif // !LOGD

#ifndef LOGD_FORMAT
#define LOGD_FORMAT(format, ...) redtea::common::rlog.d<<redtea::common::StringFormat(format, __VA_ARGS__)<<redtea::common::endl
#endif

#ifndef LOGE
#define LOGE(message) redtea::common::rlog.e<<##message##<<redtea::common::endl
#endif // !LOGE

#ifndef LOGE_FORMAT
#define LOGE_FORMAT(format, ...) redtea::common::rlog.e<<redtea::common::StringFormat(format, __VA_ARGS__)<<redtea::common::endl
#endif

#ifndef LOGI
#define LOGI(message) redtea::common::rlog.i<<##message##<<redtea::common::endl
#endif // !LOGI

#ifndef LOGI_FORMAT
#define LOGI_FORMAT(format, ...) redtea::common::rlog.i<<redtea::common::StringFormat(format, __VA_ARGS__)<<redtea::common::endl
#endif

#ifndef LOGW
#define LOGW(message) redtea::common::rlog.w<<##message##<<redtea::common::endl
#endif // !LOGW

#ifndef LOGW_FORMAT
#define LOGW_FORMAT(format, ...) redtea::common::rlog.w<<redtea::common::StringFormat(format, __VA_ARGS__)<<redtea::common::endl
#endif

namespace redtea
{
namespace common
{
	template<typename ... Args>
	std::string StringFormat(const std::string& format, Args ... args)
	{
		int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
		auto size = static_cast<size_t>(size_s);
		auto buf = std::make_unique<char[]>(size);
		std::snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}

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