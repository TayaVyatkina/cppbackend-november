#pragma once

#include <boost/log/trivial.hpp>     // для BOOST_LOG_TRIVIAL

namespace logging = boost::log;

namespace boost_log {

void MyFormatter(logging::record_view const& rec, logging::formatting_ostream& strm);

void InitBoostLogFilter();

} // namespace boost_log
