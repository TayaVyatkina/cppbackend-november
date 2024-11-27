#pragma once
#include <boost/log/trivial.hpp>     
#include <boost/log/core.hpp>        
#include <boost/log/utility/setup/console.hpp>

#include "logging_handler.h"

namespace logger {

    namespace logging = boost::log;
    namespace keywords = boost::log::keywords;
    namespace expr = logging::expressions;
    namespace json = boost::json;
    using namespace std::literals;

    void InitLogger();

    template <class T>
    std::string CreateLogMessage(std::string_view msg, T&& data) {
        return json::serialize(json::value_from(LogMessage<T>(msg, std::forward<T>(data))));
    };

}