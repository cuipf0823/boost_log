#ifndef SIMPLE_LOG_H
#define SIMPLE_LOG_H

namespace simple_log
{


enum SeverityLevel : char
{
	normal,
	notification,
	warning,
	error,
	critical
};

void Init();

//日志创建与写入
void PutLogRecords();
void AddCommonAttrs();
void CustomLog();

void TestSimple();

}
#endif