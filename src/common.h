#ifndef COMMON_H
#define COMMON_H

#ifndef BOOST_BUILD_PCH_ENABLED
#define BOOST_BUILD_PCH_ENABLED
#endif // !BOOST_BUILD_PCH_ENABLED


#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/exception_handler.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/move/utility_core.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <boost/thread/pthread/shared_mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/lock_types.hpp>
#include <boost/thread/locks.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;

#endif