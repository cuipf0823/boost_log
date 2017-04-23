#include "detail.h"

namespace detail_log
{

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", uint32_t)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", attrs::timer::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(thread_id, "ThreadID", attrs::current_thread_id::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", DetailLog::SeverityLevel)
BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope::value_type)

DetailLog::DetailLog() : file_sink_(boost::make_shared<TextSink>()), 
console_sink_(boost::make_shared<TextSink>())
{
	//设置输出到文件Detail.log中
	file_sink_->locked_backend()->add_stream(boost::make_shared< std::ofstream >("detail.log"));
	file_sink_->set_formatter(&Formatter);
	//file_sink_->set_filter(&Filter);
	//设置输出到控制台
	console_sink_->locked_backend()->add_stream(boost::make_shared<std::ostream>(std::cout.rdbuf()));
	console_sink_->set_formatter(&Formatter);
	logging::core::get()->add_sink(file_sink_);
	logging::core::get()->add_sink(console_sink_);
	logging::add_common_attributes();
}

DetailLog::~DetailLog()
{

}

void DetailLog::Formatter(const logging::record_view& view, logging::formatting_ostream& strm)
{
	strm << std::hex << std::setw(8) << std::setfill('0') << expr::attr<uint32_t>("LineID") << std::dec << std::setfill(' ');
//	strm << std::hex << std::setw(8) << std::setfill('0') << logging::extract<unsigned int>("LineID", view) << std::dec << std::setfill(' ');

 	strm << view[timestamp] << std::setfill(' ');
 	strm << std::hex << std::setw(8) << view[thread_id] << std::setfill(' ');
 	strm << view[expr::smessage];
}

void DetailLog::Filter(const SeverityLevel& level)
{

}

void DetailLog::Log(const char* msg)
{
	BOOST_LOG(lg_) << msg;
}

void DetailLog::PrintAttrs()
{
	const logging::record& rec = lg_.open_record();
	const logging::attribute_value_set& values = rec.attribute_values();
	for (const auto& iter : values)
	{

	}
}

void TestDetail()
{
	DetailLog lg;
	lg.Log("test detail log");
}

}