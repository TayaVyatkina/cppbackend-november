#include <iostream>
#include <string>
#include <string_view>

#include <boost/asio.hpp>

namespace net = boost::asio;
using net::ip::tcp;

using namespace std::literals;

int main() {
    static const int port = 3333;

    net::io_context io_context;

    // используем конструктор tcp::v4 по умолчанию для адреса 0.0.0.0
    //tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    tcp::acceptor acceptor(io_context, tcp::endpoint(net::ip::make_address("127.0.0.0"), port));
    std::cout << "Waiting for connection..."sv << std::endl;

    boost::system::error_code ec; // объект для сохранения кода ошибки
    // после принятия соединения сокет можно использовать для получения и отправления данных
    // он привязан к установленному соединению и становится для нас интерфейсом этого соединения
    tcp::socket socket{io_context};
    // метод accept заставит программу ждать, пока кто-то не подключится к серверу
    // по указанному порту
    acceptor.accept(socket, ec);

    if (ec) {
        std::cout << "Can't accept connection"sv << std::endl;
        return 1;
    }

    // Прочтём из сокета одну строку, используя функцию read_until
    net::streambuf stream_buf;// результат сохранится в объект streambuf
    // хотим прочитать строку вплоть до символа \n
    // Вместо одиночного символа эта функция может принимать регулярное выражение
    net::read_until(socket, stream_buf, '\n', ec);
    // объект streambuf преобразуется в std::string парой итераторов
    // Операция синхронная — она будет ждать пока все нужные данные не будут прочитаны.
    std::string client_data{std::istreambuf_iterator<char>(&stream_buf),
                            std::istreambuf_iterator<char>()};

    if (ec) {
        std::cout << "Error reading data"sv << std::endl;
        return 1;
    }

    std::cout << "Client said: "sv << client_data << std::endl;

    // метод write_some отправит данные через сокет противоположной стороне
    // в него передаётся буфер, который можно сконструировать из разных объектов
    // Буфер конструирует из std::string_view функция boost::asio::buffer

    // она может принимать const void* с количеством передаваемых байт
    // Этот способ допустимо использовать для передачи данных произвольного объекта.
    socket.write_some(net::buffer("Hello, I'm server!\n"sv), ec);

    if (ec) {
        std::cout << "Error sending data"sv << std::endl;
        return 1;
    }
    // Противоположная сторона может прочитать данные в изменяемый буфер,
    // который получен из неконстантного указателя на объект той же функцией boost::asio::buffer
}