// Boost.Beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp> 

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <iostream> 
#include <thread>
#include <optional>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;

namespace beast = boost::beast;
namespace http = beast::http;
// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;

// Ответ, тело ответа шаблонного класса http::response представлено классом std::string.
using StringResponse = http::response<http::string_body>;

// выводит содержимое запроса, полученного от клиента
void DumpRequest(const StringRequest& req) {
    std::cout << req.method_string() << ' ' << req.target() << std::endl;
    // Выводим заголовки запроса
    for (const auto& header : req) {
        std::cout << "  "sv << header.name_string() << ": "sv << header.value() << std::endl;
    }
}

//Если клиент отправил очередной запрос, вернёт StringRequest, 
//если клиент завершил соединение, вернёт std::nullopt. 
//О прочих ошибках функция будет сигнализировать исключениями.
std::optional<StringRequest> ReadRequest(tcp::socket& socket, beast::flat_buffer& buffer) {
    beast::error_code ec;
    StringRequest req;
    // boost::beast::http::read считывает HTTP-сообщение из потока. 
    //Роль потока в вашем случае будет играть сокет, а роль сообщения — StringRequest
    // Считываем из socket запрос req, используя buffer для хранения данных.
    // В ec функция запишет код ошибки.
    http::read(socket, buffer, req, ec);

    if (ec == http::error::end_of_stream) {
        return std::nullopt;
    }
    if (ec) {
        throw std::runtime_error("Failed to read request: "s.append(ec.message()));
    }
    return req;
}

// Структура ContentType задаёт область видимости для констант,
// задающий значения HTTP-заголовка Content-Type
struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

// Создаёт StringResponse с заданными параметрами
StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                  bool keep_alive,
                                  std::string_view content_type = ContentType::TEXT_HTML) {
    StringResponse response(status, http_version);
    response.set(http::field::content_type, content_type);
    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(keep_alive);
    if(status == http::status::method_not_allowed){
        response.set(http::field::allow, "GET, HEAD"sv);
    }
    return response;
}

StringResponse HandleRequest(StringRequest&& req) {
    const auto text_response = [&req](http::status status, std::string_view text = "") {
        return MakeStringResponse(status, text, req.version(), req.keep_alive());
    };

    // Здесь можно обработать запрос и сформировать ответ
    if (req.method() == http::verb::get){
        return text_response(http::status::ok, "Hello, "s.append(req.target().substr(1)) );
    }
    else if (req.method() == http::verb::head){
        return text_response(http::status::ok);
    }
    else{
        return text_response(http::status::method_not_allowed, "Invalid method");
    }
}

template <typename RequestHandler>
//в цикле считывать и обрабатывать запросы клиента. 
void HandleConnection(tcp::socket& socket, RequestHandler&& handle_request) {
    try {
        // Буфер для чтения данных в рамках текущей сессии.
        // boost::beast::flat_buffer способен динамически изменять свой размер
        // состоит из двух смежных областей памяти: область для чтения и область для записи данных.
        beast::flat_buffer buffer;

        // Продолжаем обработку запросов, пока клиент их отправляет
        //Цикл обработки запроса продолжается до тех пор, пока ReadRequest не вернёт std::nullopt, 
        //либо метод need_eof объекта http::response не вернёт true.
        while (auto request = ReadRequest(socket, buffer)) {
            //выведите информацию о запросе 
            DumpRequest(*request);
            // Формируем ответ со статусом 200 и версией равной версии запроса
            StringResponse response = handle_request(*std::move(request));
            // Отправляем ответ сервера клиенту
            http::write(socket, response);

            // Прекращаем обработку запросов, если семантика ответа требует это
            if (response.need_eof()) {
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    beast::error_code ec;
    //Перед тем, как выйти из функции вызовите метод tcp::socket::shutdown, 
    //чтобы запретить последующую отправку данных в этот сокет — HTTP-сессия завершена.
    // Запрещаем дальнейшую отправку данных через сокет
    socket.shutdown(tcp::socket::shutdown_send, ec);
}

int main() {
    // Контекст для выполнения синхронных и асинхронных операций ввода/вывода
    net::io_context ioc;
    const auto address = net::ip::make_address("0.0.0.0");
    constexpr unsigned short port = 8080;

    // Объект, позволяющий принимать tcp-подключения к сокету
    tcp::acceptor acceptor(ioc, {address, port});
    std::cout << "Server has started..."sv << std::endl;
    while (true) {
        tcp::socket socket(ioc);
        acceptor.accept(socket);
        // Запускаем обработку взаимодействия с клиентом в отдельном потоке
        std::thread t(
            // Лямбда-функция будет выполняться в отдельном потоке
            [](tcp::socket socket) {
                HandleConnection(socket, HandleRequest);
            },
            std::move(socket));  // Сокет нельзя скопировать, но можно переместить

        // После вызова detach поток продолжит выполняться независимо от объекта t
        t.detach();
    }
}