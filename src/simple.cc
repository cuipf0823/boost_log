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

void TestSimple()
{
	Init();
	BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	BOOST_LOG_TRIVIAL(error) << "An error severity message";
	BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
}





}