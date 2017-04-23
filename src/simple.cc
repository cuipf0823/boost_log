#include "common.h"
#include "simple.h"

namespace simple_log
{

void Init()
{
	logging::add_file_log(
		keywords::file_name = "sample_%N.log",
		keywords::rotation_size = 10 * 1024 * 1024,
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = "[%TimeStamp%]: %Message%"
		);
	logging::add_file_log("other_sample.log");
	//logging::core::get() returns a pointer to the core singleton
	logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
	
}

//声明全局logger 对象
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(global_logger, src::logger_mt);

void PutLogRecords()
{
	logging::add_common_attributes();
	//获取logger实例
	src::logger_mt& lg = global_logger::get();
	BOOST_LOG(lg) << "global macro write log records";
	//log写入
	logging::record rec = lg.open_record();
	if (rec)
	{
		logging::record_ostream strm(rec);
		strm << "global logger write";
		strm.flush();
		lg.push_record(boost::move(rec));
	}
}

void CustomLog()
{
	src::severity_logger<SeverityLevel> slg;
	BOOST_LOG_SEV(slg, error) << "error";
	BOOST_LOG_SEV(slg, warning) << "warning";
	BOOST_LOG_SEV(slg, critical) << "critical";
}

void AddCommonAttrs()
{
	boost::shared_ptr<logging::core> core = logging::core::get();
	core->add_global_attribute("LineID", attrs::counter<uint32_t>(1));
	core->add_global_attribute("TimeStamp", attrs::utc_clock());
	core->add_global_attribute("Scope", attrs::named_scope());

}




void TestSimple()
{
// 	Init();
// 	BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
// 	BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
// 	BOOST_LOG_TRIVIAL(info) << "An informational severity message";
// 	BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
// 	BOOST_LOG_TRIVIAL(error) << "An error severity message";
// 	BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
//	PutLogRecords();
	CustomLog();
}







}