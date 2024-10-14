#pragma once

#include "request_handler.h"
#include <boost/beast/http.hpp>
#include <chrono>

#include <boost/log/trivial.hpp> 
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/json.hpp>
namespace json = boost::json;
namespace logging = boost::log;

BOOST_LOG_ATTRIBUTE_KEYWORD(additional_data, "AdditionalData", json::value)

namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;
using namespace std::chrono;
using StringRequest = http::request<http::string_body>;

namespace log_handler {

class LoggingRequestHandler {
public:
    LoggingRequestHandler(http_handler::RequestHandler& request_handler)
    : logging_handler_(request_handler)
    {}

    template <typename Body, typename Allocator, typename Send>
    void operator()(std::string&& end_point, http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        LogRequest(end_point, req);

        const auto start_time = steady_clock::now();
        auto sender = [&send, self = this, start_time](auto&& res){
            self->LogResponse(res, start_time);
            send(std::forward<decltype(res)>(res));
        };

        // Обработать запрос request и отправить ответ, используя send
        logging_handler_(std::forward<decltype(req)>(req), std::move(sender));
    }


private:
    http_handler::RequestHandler& logging_handler_;

    static std::string GetNameMetod(http::verb method){
        switch(method){
        case http::verb::get: 
            return "GET"s;
        case http::verb::head: 
            return "HEAD"s;
        default: 
            return "UNKNOWN"s;
        }
    }

    static void LogRequest(std::string end_point, const StringRequest& req){
        auto target = req.target();
        std::string uri(target.data(), target.size());
        // http::verb method = req.method();
        json::value custom_data{
              {"ip"s, end_point}
            , {"URI"s, uri}
            , {"method"s, GetNameMetod(req.method())}
        };
        BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, custom_data)
                                << "request received"sv;
    }

    template <typename Body>
    void LogResponse(const http::response<Body>& response, const std::chrono::_V2::steady_clock::time_point start_time){
        const auto duration = std::chrono::steady_clock::now() - start_time;
        json::value custom_data{
              {"response_time"s, duration_cast<std::chrono::milliseconds>(duration).count()}
            , {"code"s, response.result_int()}
            , {"content_type"s, ""s}
        };
        if (response.base().count(http::field::content_type) == 0){
            custom_data.as_object().at("content_type"s) = nullptr;           
        } else {  
            custom_data.as_object().at("content_type"s) = std::string(response.base().at(http::field::content_type));         
        };
        BOOST_LOG_TRIVIAL(info) << logging::add_value(additional_data, custom_data)
                                << "response sent"sv;
    }
};

}// namespace logging_request_handler
