#pragma once
#include "http_server.h"
#include "model.h"

#include <filesystem>
#include <memory>
#include <variant>

namespace http_handler {

namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;
namespace fs = std::filesystem;

// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = http::response<http::string_body>;
// Ответ, тело которого представлено в виде файла
using FileResponse = http::response<http::file_body>;

using Response = std::variant<StringResponse, FileResponse>;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    constexpr static std::string_view TEXT_PLAIN = "text/plain"sv;
    constexpr static std::string_view APP_JSON = "application/json"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

// Создаёт StringResponse с заданными параметрами
StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                bool keep_alive, http::verb method,
                                std::string_view content_type = ContentType::APP_JSON);


class RequestHandler {
public:
    explicit RequestHandler(model::Game& game, fs::path path)
        : game_{game}
        , static_content_path_(std::move(path)) {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
        // Обработать запрос request и отправить ответ, используя send
        //send(HandleRequest(std::move(req)));
        auto response = HandleRequest(std::move(req));
        if (std::holds_alternative<StringResponse>(response)) {
            send(std::get<StringResponse>(response));
        } 
        else {
            send(std::get<FileResponse>(response));
        }     
    }

private:
    model::Game& game_;

    Response HandleRequest(StringRequest&& req);    
    fs::path static_content_path_;
};

}  // namespace http_handler