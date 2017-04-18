#ifndef BOOST_LOG_H
#define BOOST_LOG_H
namespace complex_log
{

//We define our own severity levels
enum severity_level
{
	normal,
	notification,
	warning,
	error,
	critical
};

void logging_function();
void named_scope_logging();
void tagged_logging();
void timed_logging();
void Init();
void TestComplex();

}
#endif