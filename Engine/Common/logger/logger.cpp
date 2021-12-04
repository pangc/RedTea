#include "logger.h"
namespace redtea
{
namespace common
{
	LoggerStream::LoggerStream(Priority p) noexcept
	: mPriority(p)
	{
		info = stdout;
		error = stderr;
	}

	ostream& LoggerStream::flush() noexcept {
		Buffer& buf = getBuffer();
		switch (mPriority) {
		case LOG_DEBUG:
		case LOG_WARNING:
		case LOG_INFO:
			fprintf(info, "%s", buf.get());
			break;
		case LOG_ERROR:
			fprintf(error, "%s", buf.get());
			break;
		}
		buf.reset();
		return *this;
	}

	void LoggerStream::SetOutputFile(std::string file)
	{
		FILE* fp = fopen(file.c_str(), "w+");
		info = fp;
		error = fp;
	}

	static LoggerStream cout(LoggerStream::Priority::LOG_DEBUG);
	static LoggerStream cerr(LoggerStream::Priority::LOG_ERROR);
	static LoggerStream cwarn(LoggerStream::Priority::LOG_WARNING);
	static LoggerStream cinfo(LoggerStream::Priority::LOG_INFO);

	Logger const rlog = {
		cout,   // debug
		cerr,   // error
		cwarn,  // warning
		cinfo   // info
	};
}
}