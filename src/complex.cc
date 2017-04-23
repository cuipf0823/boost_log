#include "common.h"
#include "complex.h"

namespace complex_log
{
//placeholder
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", SeverityLevel)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(timeline, "Timeline", attrs::timer::value_type)

std::ostream& operator<< (std::ostream& strm, SeverityLevel level)
{
	static const char* strings[] =
	{
		"normal",
		"notification",
		"warning",
		"error",
		"critical"
	};

	if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
	{
		strm << strings[level];
	}
	else
	{
		strm << static_cast<int>(level);
	}

	return strm;
}

void LoggingFunction()
{
	src::severity_logger< SeverityLevel > slg;
	BOOST_LOG_SEV(slg, normal) << "A regular message";
	BOOST_LOG_SEV(slg, warning) << "Something bad is going on but I can handle it";
	BOOST_LOG_SEV(slg, critical) << "Everything crumbles, shoot me now!";
}

//添加scope name 对每一个log record
void NamedScopeLogging()
{
	BOOST_LOG_NAMED_SCOPE("named_scope_logging");
	src::severity_logger< SeverityLevel > slg;
	BOOST_LOG_SEV(slg, normal) << "Hello from the function named_scope_logging!";
}

//添加自定义tag
void TaggedLogging()
{
	src::severity_logger<SeverityLevel> slg;
	slg.add_attribute("Tag", attrs::constant< std::string >("Tag Value"));
	BOOST_LOG_SEV(slg, normal) << "Here goes the tagged record";
}

void TimedLogging()
{
	BOOST_LOG_SCOPED_THREAD_ATTR("Timeline", attrs::timer());
	src::severity_logger<SeverityLevel> slg;
	BOOST_LOG_SEV(slg, normal) << "Starting to time nested functions";
	LoggingFunction();
	BOOST_LOG_SEV(slg, normal) << "Stopping to time nested functions";
}

void Init()
{
	typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
	sink->locked_backend()->add_stream(boost::make_shared< std::ofstream >("complex.log"));
	sink->set_formatter
		(
		expr::stream
		<< std::hex << std::setw(8) << std::setfill('0') << line_id << std::dec << std::setfill(' ')
		<< ": <" << severity << ">\t"
		<< "(" << scope << ") "
		<< expr::if_(expr::has_attr(tag_attr))
		[
			expr::stream << "[" << tag_attr << "] "
		]
		<< expr::if_(expr::has_attr(timeline))
		[
			expr::stream << "[" << timeline << "] "
		]
		<< expr::smessage
		);
	//同时输出到console上
	sink->locked_backend()->add_stream(boost::make_shared<std::ostream>(std::cout.rdbuf()));

	logging::core::get()->add_sink(sink);
	logging::add_common_attributes();
	logging::core::get()->add_global_attribute("Scope", attrs::named_scope());
}

struct PrintVisitor
{
	typedef void ResultValue;
	ResultValue operator()(SeverityLevel level) const
	{
		std::cout << "PrintVisitor: " << level << std::endl;
	}
};

void PrintSeverityVisit(const logging::record& rec)
{
	logging::visit<SeverityLevel>("Severity", rec, 
		[=](SeverityLevel level) { std::cout << "PrintSeverityVisit Lambda: "<< level << std::endl; });
	logging::visit<SeverityLevel>("Severity", rec, PrintVisitor());
}

void PrintSeverityExtract(const logging::record& rec)
{
	logging::value_ref<SeverityLevel> level = logging::extract<SeverityLevel>("Severity", rec);
	std::cout << "PrintSeverityExtract: " << level << std::endl;
}

void PrintSeveritySubscript(const logging::record& rec)
{
	logging::value_ref<SeverityLevel, tag::severity> level = rec[severity];
	std::cout << "PrintSeveritySubscript: " << level << std::endl;
	logging::value_ref<unsigned int, tag::line_id> line = rec[line_id];
	std::cout << "PrintSeveritySubscript: " << line << std::endl;
}

void PrintSeverityLookup(const logging::record& rec)
{
	const logging::attribute_value_set& values = rec.attribute_values();
	auto iter = values.find("Severity");
	if (iter != values.end())
	{
		const logging::attribute_value& value = iter->second;
		std::cout << value.extract<SeverityLevel>() << std::endl;
	}
}

void MassLog()
{
	src::severity_logger<SeverityLevel> slg;
	const auto& rec = slg.open_record();
	for (int idx = 0; idx < 20; ++idx)
	{
		BOOST_LOG_SEV(slg, error) << "Mass log test number: " << idx; 
		PrintSeverityVisit(rec);
		PrintSeverityExtract(rec);
		PrintSeveritySubscript(rec);
		PrintSeverityLookup(rec);
	}
	
}

void TestComplex()
{
	Init();
// 	NamedScopeLogging();
// 	TaggedLogging();
// 	TimedLogging();
	MassLog();
}
}