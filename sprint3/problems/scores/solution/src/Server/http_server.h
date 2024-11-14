#pragma once
#include "sdk.h"
#include "logger.h"
// boost.beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http_server {



namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;
namespace sys = boost::system;

using namespace std::literals;

void ErrorMessage(beast::error_code ec, std::string_view what);

class SessionBase {
public:
     
    void Run();                            

    const std::string& GetRemoteIp() {  
        static std::string remote_ip;
        try {
            auto temp = stream_
                .socket()
                .remote_endpoint()
                .address()
                .to_string();
            remote_ip = temp;
        }
        catch (...) {}
        return remote_ip;
    };

protected:


    using HttpRequest = http::request<http::string_body>;
    
    SessionBase(const SessionBase&) = delete;                           
    SessionBase& operator=(const SessionBase&) = delete;                
    ~SessionBase() = default;                                        
    explicit SessionBase(tcp::socket && socket);                        

    template <typename Body, typename Fields>
    void Write(http::response<Body, Fields> && response) {
        auto safe_response = std::make_shared<http::response<Body, Fields>>(std::move(response));

        auto self = GetSharedThis();
        http::async_write(stream_, *safe_response,
            [safe_response, self](beast::error_code ec, std::size_t bytes_written) {
                self->OnWrite(safe_response->need_eof(), ec, bytes_written);
                BOOST_LOG_TRIVIAL(info) << logger::CreateLogMessage("response sent"sv,
                    logger::ResponseLog<Body, Fields>(self->GetRemoteIp(),
                        self->GetDurReceivedRequest(boost::posix_time::microsec_clock::local_time()),
                        *safe_response));
            });
    }

    void SetReqRecieveTime(const boost::posix_time::ptime& reqTime) {                  
        reqRecieveTime_ = reqTime;
    }

    long GetDurReceivedRequest(const boost::posix_time::ptime& currTime) {             
        boost::posix_time::time_duration dur = currTime - reqRecieveTime_;
        return dur.total_milliseconds();
    }

private:
    
    void Read();                                                                        
    void OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read);
    void Close();                                                                       
    void OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written);
    virtual void HandleRequest(HttpRequest && request) = 0;                             
    virtual std::shared_ptr<SessionBase> GetSharedThis() = 0;                           
    boost::posix_time::ptime reqRecieveTime_;                                           
    
    beast::tcp_stream stream_;                                                         
    beast::flat_buffer buffer_;                                                         
    HttpRequest request_;                                                               
};

template <typename RequestHandler>
class Session : public SessionBase, public std::enable_shared_from_this<Session<RequestHandler>> {
public:
    
    template <typename Handler>
    Session(tcp::socket&& socket, Handler&& request_handler)                            
        : SessionBase(std::move(socket))
        , request_handler_(std::forward<Handler>(request_handler)) {
    }

private:
    void HandleRequest(HttpRequest&& request) override {
        SetReqRecieveTime(boost::posix_time::microsec_clock::local_time());
        BOOST_LOG_TRIVIAL(info) << logger::CreateLogMessage("request received"sv,
            logger::RequestLog(GetRemoteIp(), request));
        request_handler_(std::move(request), [self = this->shared_from_this()](auto&& response) {
            self->Write(std::move(response));
        });
    }

    
    std::shared_ptr<SessionBase> GetSharedThis() override {                           
        return this->shared_from_this();
    }

    RequestHandler request_handler_;
};

template <typename RequestHandler>
class Listener : public std::enable_shared_from_this<Listener<RequestHandler>> {
public:
    template <typename Handler>
    Listener(net::io_context& ioc, const tcp::endpoint& endpoint, Handler&& request_handler)
        : ioc_(ioc)
        , acceptor_(net::make_strand(ioc))                                              
        
        acceptor_.open(endpoint.protocol());                                           
        acceptor_.set_option(net::socket_base::reuse_address(true));                   
        acceptor_.bind(endpoint);                                                      
        acceptor_.listen(net::socket_base::max_listen_connections);                     
    }

    void Run() {
        DoAccept();
    }

private:
    void DoAccept() {
        acceptor_.async_accept(
            net::make_strand(ioc_),                                                     
            beast::bind_front_handler(&Listener::OnAccept, this->shared_from_this()));  
    }

    
    void OnAccept(sys::error_code ec, tcp::socket socket) {                           
        using namespace std::literals;

        if (ec) {
            return ErrorMessage(ec, "Accept Error"sv);
        }
        AsyncRunSession(std::move(socket));                                            
        DoAccept();
    }

     void AsyncRunSession(tcp::socket&& socket) {
        std::make_shared<Session<RequestHandler>>(std::move(socket), request_handler_)->Run();
    }

    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    RequestHandler request_handler_;
};

template <typename RequestHandler>
void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, RequestHandler&& handler) {
    std::make_shared<Listener<std::decay_t<RequestHandler>>>(ioc, endpoint, std::forward<RequestHandler>(handler))->Run();
}

}
