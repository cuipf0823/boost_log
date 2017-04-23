#ifndef DETAIL_LOG_H
#define DETAIL_LOG_H

#include "common.h"

namespace detail_log
{
class DetailLog
{
	typedef sinks::synchronous_sink<sinks::text_ostream_backend> TextSink;
public:
	//自定义日志等级
	enum SeverityLevel : char
	{
		normal,
		notification,
		warning,
		error,
		critical
	};

public:
	DetailLog();
	~DetailLog();
	void Log(const char* msg);
	void PrintAttrs();
private:
	static void Formatter(const logging::record_view& view, logging::formatting_ostream& strm);
	static void Filter(const SeverityLevel& level);

private:
	src::logger lg_; 
	boost::shared_ptr<TextSink> file_sink_;
	boost::shared_ptr<TextSink> console_sink_;
};

void TestDetail();

}
#endif