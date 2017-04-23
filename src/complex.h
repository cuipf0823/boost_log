#ifndef BOOST_LOG_H
#define BOOST_LOG_H
namespace complex_log
{

//We define our own severity levels
enum SeverityLevel : char	    
{
	normal,
	notification,
	warning,
	error,
	critical
};

void LoggingFunction();
void NamedScopeLogging();
void TaggedLogging();
void TimedLogging();
void Init();
void TestComplex();

}
#endif