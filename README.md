[TOC]



# 概述
## 编译配置
1. boost库 log模块是静态库形式存在, 程序链接时需要链接多个库boost_log_setup boost_log boost_thread boost_system;
2. 编译选项还要包含`-DBOOST_LOG_DYN_LINK`, 否则也会报错;
3. 包含boost相关文件, 编译速度就会巨慢, 可以使用gcc -H参数查看编译连接所有的文件, 就知道有多少了; 解决办法: 使用头文件预编译技术后稍好, 暂时没有找到更好的办法;

## 命名空间
为了代码书写方便, 以及更加整洁; 一般都会对命名空间定义一些别名, 如下:

```
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;
```


# 使用
## 基本使用
带日志等级过滤boost log的基本使用实例如下:
```C++
namespace logging = boost::log;

void Init()
{
	//logging::core::get() returns a pointer to the core singleton
	logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
}

void TestBoostLog()
{
	Init();
	BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	BOOST_LOG_TRIVIAL(error) << "An error severity message";
	BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
}
```

输出:
```
[2017-04-14 14:33:03.427867] [0x00007ffff7fd99c0] [info]    An informational severity message
[2017-04-14 14:33:03.427909] [0x00007ffff7fd99c0] [warning] A warning severity message
[2017-04-14 14:33:03.427918] [0x00007ffff7fd99c0] [error]   An error severity message
[2017-04-14 14:33:03.427925] [0x00007ffff7fd99c0] [fatal]   A fatal severity message
```
输出信息包含:
1. 时间戳;
2. 当前线程的ID;
3. 日志的等级;
4. Message, 日志内容;

**说明:**
1. Trivial 头文件可用于一般的控制台输出, 日志的默认等级也定义该头文件, 这也是boost log库默认的日志等级;
```
//! Trivial severity levels
enum severity_level
{
    trace,
    debug,
    info,
    warning,
    error,
    fatal
};
```
2. 全局日志等级过滤设置使用`logging::core::get()->set_filter`;
3. 注意`set_filter`参数, 使用的是[Boost.Phoenix](http://www.boost.org/doc/libs/1_63_0/libs/phoenix/doc/html/index.html)  lambda表达式; 表达式的左边参数描述一个等待被校验的属性, 表达式的右边是需要被校验的值;
4. `logging::core::get()` 返回是一个指针指向core单例的;

## 日志输出
上例日志直接输出在控制台, 服务端日志直接输出到控制台肯定不可取, boost log支持自定义输出, 通过构造Sinks注册到logging code实现;

注意: 上例子中, 没有初始化任何sink, 依然可以运行, 这是因为log库包含一个默认的sink, 此sink以固定的格式打印到屏幕上;

### 文件输出

```
void Init()
{
	logging::add_file_log(
		keywords::file_name = "sample_%N.log",
		keywords::rotation_size = 1 * 1024 * 1024,
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = "[%TimeStamp%]: %Message%"           
		);
	//logging::core::get() returns a pointer to the core singleton
	logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
}
```
说明:
* 我们可以使用`logging::add_file_log`初始化sink, 保存log record 到文件中;
* 该函数可以设置一些自定义选项:
    1. 日志文件的命名方式;
    2. 每1mb滚动一次log文件;
    3. 根据时间来滚动log文件;
    4. log输出的格式;
* 可以通过该函数注册多个sink, 每一个sink都可以接受处理日志, 且之间相互独立;

### 多种输出方式
```C++
void Init()
{
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    //设置输出到文件complex.log中
	boost::shared_ptr< text_sink > sink = boost::make_shared<text_sink>();
	sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>("complex.log"));

	//同时输出到console上
	sink->locked_backend()->add_stream(boost::make_shared<std::ostream>(std::cout.rdbuf()));
	//输出到console上与上面等价
	boost::shared_ptr<std::ostream> stream_ptr(&std::clog, boost::null_deleter());
	sink->locked_backend()->add_stream(stream_ptr);
	logging::core::get()->add_sink(sink);
}
```
这种方式, 和方式1中效果完全一样; 这里简要介绍一下sink, 后续有详细的介绍;
* sink 分为前端(frontend)和后端(backend)sink;
* frontend sink(上述代码中synchronous_sink类模板)负责sink的通用任务; 如: 线程同步模式, 过滤, 格式化等;
* 后端(在上述代码中使用text_ostream_backend类)实现sink特定的事情，如写到一个文件。
* boost log库提供了许多可直接使用的frontend sinks and backend sinks。    
* synchronous_sink 表明sink是线程安全的, 即backend sink不用担心线程安全问题;
* text_ostream_backend 类可以将格式化的log写入到各种stream中; 上面分别展现了输出到文件, 以及两种输出到console的方式;
* 注册多个sink与注册一个sink但包含多个目标stream是有区别的; 对于前者sink之间相互独立; 后者效率较高;
* locked_backend成员函数是线程安全的; 函数返回是一个指向backend的智能指针; **注意:** 该智能指针存在期间, backend一直是锁住状态; 也就意味着其他线程想要把日志写到该backend sink上将会被阻塞, 直到backend sink被释放;

boost log库提供了多个用于处理各种日志逻辑的backends sinks; 例如: 你可以将日志发送(by network)到syslog 服务器通过指定syslog backend; 你也可以设置Windows NT Event log backend来监控你应用的运行时间; 等等;

## 日志创建与写入
上面我们简单说了一下日志如何存储以及存储在哪里的问题; 接下来了解一下如何创建日志源以及如何写日志 ?
```
src::logger lg;
```
1. 日志源sources不需要想sinks一样被注册, sources直接和log core直接交互;
2. boost log 提供两个版本的logger, 线程安全版和非线程安全版;
3. boost log库提供大量包含不同特征的loggers; 如日志等级以及对channel支持; 这些特征可以自由相互组合构成不同功能的logger;

### 全局logger对象
log库提供了一种声明全局loggers对象的方法:

```
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(global_logger, src::logger_mt)
```
说明:
1. global_logger 用户自定义的logger; 后续可以使用该global_logger获取logger实例;
>     //获取全局唯一的logger实例
>     src::logger_mt& lg = my_logger::get();
2. logger_mt 线程安全logger;

### 写日志

无论你使用的是哪一种logger(类成员变量模式还是全局模式, 线程安全或不安全), 都可以使用一下方式写日志:
```
//声明全局logger 对象
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(global_logger, src::logger_mt);

void PutLogRecords()
{
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
```
输出:
```
[2017-04-19 15:38:01.191468] [0x00007ffff7fd8740] [info]    global macro write log records
[2017-04-19 15:38:01.191551] [0x00007ffff7fd8740] [info]    global logger write
```
说明:
1. 鼓励使用BOOST_LOG宏进行日志写入;
2. open_record调用可以用于判断record是否被创建以及至少被一个sinks处理;
3. 过滤也可以在这个阶段设置;

## 日志属性
* 每一个log record都有多个命名的属性, 这些属性代表着日志记录的条件: 在代码中的位置, 执行模块的名字, 当前时间, 或者是与你程序相关的任何数据和运行环境.
* 每一个属性都会对应一个属性值, 可以供过滤, 格式化和sinks使用; boost log库中自带大量用户常用的属性.
* 属性分为三种:　source-specific, thread-specific 和 global；　当log record形成, 与之相关这三种组成一个集合传递到sinks中;
* 每一个属性需要一个独一无二的名字, 方便检索; 如果在不同的作用域存在同名属性, 覆盖次序: 源相关属性 > 线程相关属性 > 全局属性; 这样可以通过注册本地属性来覆盖全局和线程相关属性, 以减少线程干扰;

### 属性注册
#### 公用属性
一些属性是任何程序都会使用到的, 比如: 日志记录个数, 时间等等; 这些公用的属性可以通过下面的函数添加:
```
logging::add_common_attributes();
```
通过该函数调用, 日志行ID, 时间戳, 进程ID, 线程ID都会被注册; 当然, 单线程模式下, 线程ID没有被注册.

**注意: **
1. 程序启动默认, 没有注册任何属性, 可以在程序初始化时完成属性注册; 之前Trivial logging实例由于使用的默认sink, 所以包含日志等级等默认属性, 因此不需要初始化;
2. 一旦你使用过滤, 格式化, 以及非默认sink, 你就必须注册必要的属性.


还有一些属性在logger构造时候会被自动注册, 例如: 日志的等级.

#### 更多属性
```
//自定义log安全属性
void logging_function()
{
	src::severity_logger< severity_level > slg;
	BOOST_LOG_SEV(slg, normal) << "A regular message";
	BOOST_LOG_SEV(slg, warning) << "Something bad is going on but I can handle it";
	BOOST_LOG_SEV(slg, critical) << "Everything crumbles, shoot me now!";
}

//添加scope name 对每一个log record
void named_scope_logging()
{
	BOOST_LOG_NAMED_SCOPE("named_scope_logging");
	src::severity_logger< severity_level > slg;
	BOOST_LOG_SEV(slg, normal) << "Hello from the function named_scope_logging!";
}

//添加自定义tag
void tagged_logging()
{
	src::severity_logger< severity_level > slg;
	slg.add_attribute("Tag", attrs::constant< std::string >("My tag value"));
	BOOST_LOG_SEV(slg, normal) << "Here goes the tagged record";
}
```
说明:
1. 通过添加自定义tag属性, 可以用于后续的格式化以及过滤;
2. 添加scope name可以标记应用程序不同部分产生log;

此外, 也可以使用boost log添加Timeline属性, 构成一个简单的profiling工具来检测性能, Timeline包含高精度的时间信息，可以用来判断哪一部分程序需要很多的执行时间。如下:
```
void timed_logging()
{
	BOOST_LOG_SCOPED_THREAD_ATTR("Timeline", attrs::timer());
	src::severity_logger< severity_level > slg;
	BOOST_LOG_SEV(slg, normal) << "Starting to time nested functions";
	logging_function();
	BOOST_LOG_SEV(slg, normal) << "Stopping to time nested functions";
}
```
输出:
```
00000003: <normal>	() [00:00:00.000005] Starting to time nested functions
00000004: <normal>	() [00:00:00.000176] A regular message
00000005: <warning>	() [00:00:00.000230] Something bad is going on but I can handle it
00000006: <critical>() [00:00:00.000267] Everything crumbles, shoot me now!
00000007: <normal>	() [00:00:00.000301] Stopping to time nested functions
```
### 属性占位符
属性占位符主要在接下来的部分会用到,  每一个属性指定一个关键词, 这个关键词在日志过滤和格式化中会被用到;
```
//placeholder
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(scope, "Scope", attrs::named_scope::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(timeline, "Timeline", attrs::timer::value_type)
```
说明:
1. 宏的第一个参数就是keyword, 第二个参数是属性名, 第三个参数属性值类型;

## 日志格式化
如果之前的实例(不包含使用默认输出的例子), 没有添加格式化, 输出的日志信息就只有Message信息;

设置方法:
1. `add_file_log`中通过`keyword::format`设置;
2. 如果手动设置了sink, 可以通过sink的set_formatter来设置;

### lambda 风格格式化
```
void init()
{
    logging::add_file_log
    (
        keywords::file_name = "sample_%N.log",
        // 日志输出格式
        // 1: <normal> A normal severity message
        keywords::format =
        (
            expr::stream
                << expr::attr< unsigned int >("LineID")
                << ": <" << logging::trivial::severity
                << "> " << expr::smessage
        )
    );
}
```
如上, 这里格式化使用的都是占位符, 我们可以在过滤器和格式化工具中使用同样的占位符; attr占位符和severity占位符类似, 它们表示属性值; 不同的是severity占位符表示名称为”Severity”和类型为
trivial::severity_level的属性，attr可以表示任何属性。
我们也可以使用下面方式代替severity占位符:
```
expr::attr< logging::trivial::severity_level >("Severity")
```

也可以提供一些其他属性的初始化如日期, 时间, LineID等等;
```
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
```

### Boost.Format 风格格式化
```
void init()
{
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();
    sink->locked_backend()->add_stream(boost::make_shared< std::ofstream >("sample.log"));
    // 日志输出格式
    // 1: <normal> A normal severity message
    // 2: <error> An error severity message
    sink->set_formatter
    (
        expr::format("%1%: <%2%> %3%")
            % expr::attr< unsigned int >("LineID")
            % logging::trivial::severity
            % expr::smessage
    );

    logging::core::get()->add_sink(sink);
}
```
boost log格式化也支持[Boost.Format](http://www.boost.org/doc/libs/1_63_0/libs/log/doc/html/log/tutorial/formatters.html)格式, 具体的语法不在这里赘述;

### 专门的格式化
boost log库提供了专门的初始化为有些类型属性, 如日期, 时间以及命名作用域; 例如: 格式化Date和time为[Boost.DateTime](http://www.boost.org/doc/libs/1_64_0/doc/html/date_time.html)格式.
```
void init()
{
    logging::add_file_log
    (
        keywords::file_name = "sample_%N.log",
        // YYYY-MM-DD HH:MI:SS: <normal> A normal severity message
        keywords::format =
        (
            expr::stream
                << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
                << ": <" << logging::trivial::severity
                << "> " << expr::smessage
        )
    );
}
```
这种对日期时间的格式化也可以与上面Boost.Format风格一起使用.

### 字符串风格格式化
有些情况下, 我们可以使用字符串模板格式化日志, 字符串中使用`%%`来标示占位符, 如: `%Message%`占位符会被日志记录message来代替;
```
void init()
{
    logging::add_file_log
    (
        keywords::file_name = "sample_%N.log",
        keywords::format = "[%TimeStamp%]: %Message%"
    );
}
```
注: sink的成员函数`set_formatter`不支持该风格格式化(字符串风格);

### 自定义格式化函数
上面几种格式化方式, 我们都是通过参数传递给具体的格式化函数; 实际上, 我们也可以定义我们自己格式化函数, 然后通过函数对象方式传递给log库格式化函数!

```
void MyFormatter(logging::record_view const& rec, logging::formatting_ostream& strm)
{
    strm << logging::extract< unsigned int >("LineID", rec) << ": ";
    strm << "<" << rec[logging::trivial::severity] << "> ";
    strm << rec[expr::smessage];
}

void init()
{
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();
    sink->locked_backend()->add_stream(boost::make_shared< std::ofstream >("sample.log"));
    sink->set_formatter(&MyFormatter);
    logging::core::get()->add_sink(sink);
}
```
说明:
* logging::record_view 和log::record相似; 区别: record_view是不可修改且实现浅拷贝; 日志格式化和过滤所有操作都是在record_view层面的, 并不会直接影响到log record, 这些record可能被其他sinks使用在其他线程中;

## 日志过滤
上面很多次都提到日志过滤, 在概述中简单使用过日志等级过滤; 较复杂的日志过滤设置如下:
```
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)

void init()
{
    logging::formatter fmt = expr::stream
        << std::setw(6) << std::setfill('0') << line_id << std::setfill(' ')
        << ": <" << severity << ">\t"
        << expr::if_(expr::has_attr(tag_attr))
           [
               expr::stream << "[" << tag_attr << "] "
           ]
        << expr::smessage;

    // Initialize sinks
    typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();
    sink->locked_backend()->add_stream(boost::make_shared< std::ofstream >("full.log"));
    sink->set_formatter(fmt);

    logging::core::get()->add_sink(sink);
    sink = boost::make_shared< text_sink >();

    sink->locked_backend()->add_stream(boost::make_shared< std::ofstream >("important.log"));
    sink->set_formatter(fmt);
    sink->set_filter(severity >= warning || (expr::has_attr(tag_attr) && tag_attr == "IMPORTANT_MESSAGE"));
    logging::core::get()->add_sink(sink);
    logging::add_common_attributes();
}
```
实例中输出到日志文件"important.log"的sink添加过滤; 通过调用函数`set_filter`实现, 函数接收参数依然是Boost.Phoenix风格;

当然你也可以像自定格式化函数一样, 自定义过滤函数:
```
bool MyFilter(logging::value_ref< severity_level, tag::severity > const& level, logging::value_ref< std::string, tag::tag_attr > const& tag)
{
    return level >= warning || tag == "IMPORTANT_MESSAGE";
}

void init()
{
    namespace phoenix = boost::phoenix;
    sink->set_filter(phoenix::bind(&my_filter, severity.or_none(), tag_attr.or_none()));.
}
```
# 详解

## 日志记录(Records)
日志记录包含所有的日志相关的信息; 这些信息(包括日志内容本身)都使用命名的属性值表示; 这些属性值可以被过滤, 格式化, sinks等功能使用;  
特别的属性值可以通过以下几种方法访问:
```
struct PrintVisitor
{
	typedef void ResultValue;
	ResultValue operator()(SeverityLevel level) const
	{
		std::cout << "PrintVisitor: " << level << std::endl;
	}
};

//使用logging::visit
void PrintSeverityVisit(const logging::record& rec)
{
    //使用lambda代替PrintVisitor
	logging::visit<SeverityLevel>("Severity", rec,
		[=](SeverityLevel level) { std::cout << "PrintSeverityVisit Lambda: "<< level << std::endl; });
	logging::visit<SeverityLevel>("Severity", rec, PrintVisitor());
}

//使用logging::extract
void PrintSeverityExtract(const logging::record& rec)
{
	logging::value_ref<SeverityLevel> level = logging::extract<SeverityLevel>("Severity", rec);
	std::cout << "PrintSeverityExtract: " << level << std::endl;
}

//直接通过下标
void PrintSeveritySubscript(const logging::record& rec)
{
	logging::value_ref<SeverityLevel, tag::severity> level = rec[severity];
	std::cout << "PrintSeveritySubscript: " << level << std::endl;
	logging::value_ref<unsigned int, tag::line_id> line = rec[line_id];
	std::cout << "PrintSeveritySubscript: " << line << std::endl;
}

//look up
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
```
