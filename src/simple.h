#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup/file.hpp>

namespace simple_log
{


namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;

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
#endif