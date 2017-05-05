#include "detail.h"
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/support/date_time.hpp>


namespace detail_log
{

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", uint32_t)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(thread_id, "ThreadID", attrs::current_thread_id::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", DetailLog::SeverityLevel)
BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope::value_type)

DetailLog::DetailLog() : file_sink_(boost::make_shared<TextSink>()), 
console_sink_(boost::make_shared<TextSink>())
{
	//设置输出到文件Detail.log中
	file_sink_->locked_backend()->add_stream(boost::make_shared< std::ofstream >("detail.log"));
	file_sink_->set_formatter(&Formatter);

	//设置输出到控制台
	console_sink_->locked_backend()->add_stream(boost::make_shared<std::ostream>(std::cout.rdbuf()));
	console_sink_->set_formatter(&Formatter);
	logging::core::get()->add_sink(file_sink_);
	logging::core::get()->add_sink(console_sink_);
	//logging::add_common_attributes();
}

DetailLog::~DetailLog()
{

}


logging::formatting_ostream& operator<< (logging::formatting_ostream& strm, DetailLog::SeverityLevel level)
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

//输出格式: LineID TimeSamp ThreadID Level Message;
void DetailLog::Formatter(const logging::record_view& view, logging::formatting_ostream& strm)
{
	strm << std::hex << std::setw(8) << std::setfill('0') << view[line_id]
		<< std::dec << std::setfill(' ') << "\t";

	strm << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S") <<"\n";
  	strm << std::hex << view[thread_id] << "\t";
 	strm << view[severity] << "\t";
  	strm << view[expr::message];
}

void DetailLog::Filter(const SeverityLevel& level)
{

}

void DetailLog::Log(const char* msg)
{
//	BOOST_LOG(lg_) << msg;
//	BOOST_LOG(lg_) << "global macro write log records";
	logging::record rec = lg_.open_record();
	if (rec)
	{
		logging::record_ostream strm(rec);
		strm << msg;
		strm.flush();
		lg_.push_record(boost::move(rec));
	}
}

void DetailLog::PrintAttrs()
{
	const logging::record& rec = lg_.open_record();
	const logging::attribute_value_set& values = rec.attribute_values();
	for (const auto& iter : values)
	{

	}
}


//basic	logger 使用
class TcpConnect
{
public:
	TcpConnect()
	{

	};
	~TcpConnect()
	{

	};
	void OnConnect(const std::string& remote_addr)
	{
		//连接成功之后会自动加载属性"RemoteAddress"到日志中
		remote_addr_ = lg_.add_attribute("RemoteAddress", attrs::constant< std::string >(remote_addr)).first;
		if (logging::record rec = lg_.open_record())
		{
			rec.attribute_values().insert("Message", attrs::make_attribute_value(std::string("Connection established")));
			lg_.push_record(boost::move(rec));
		}
	}

	void OnDisConnect()
	{
		//断开连接删除"RemoteAddress"属性
		BOOST_LOG(lg_) << "Connection shut down";
		lg_.remove_attribute(remote_addr_);
	}

	void Send(std::size_t size)
	{
		//添加额外的属性"Size"便于统计, 只对该函数有效
		BOOST_LOG(lg_) << logging::add_value("SentSize", size) << "Some data sent";
	}

	void Receive(std::size_t size)
	{
		BOOST_LOG(lg_) << logging::add_value("ReceivedSize", size) << "Some data received";
	}
private:
	src::logger lg_;
	logging::attribute_set::iterator remote_addr_;
};




void TestDetail()
{
	DetailLog dlg;
	for (int i = 0; i < 10; ++i)
	{
		dlg.Log("test detail log idx");

	}

// 	TcpConnect con_;
// 	con_.OnConnect("127.0.0.1:3000");
// 	con_.Send(100);
// 	con_.Receive(200);
// 	con_.OnDisConnect();
}

}