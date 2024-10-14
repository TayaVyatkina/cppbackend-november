#include <boost/log/trivial.hpp>
#include <boost/log/trivial.hpp>     // для BOOST_LOG_TRIVIAL
#include <boost/log/core.hpp>        // для logging::core
#include <boost/log/expressions.hpp> // для выражения, задающего фильтр 
#include <boost/log/utility/setup/file.hpp> // для записи в файл
#include <boost/log/utility/setup/common_attributes.hpp> // позволяет выводить плейсхолдер %TimeStamp%
#include <boost/log/utility/setup/console.hpp> // вывод в консоль
#include <boost/date_time.hpp> // вывод момента времени
#include <boost/log/utility/manipulators/add_value.hpp> //Для задания атрибутов при логировани
//#include
#include <string_view>

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)// тип параметра 
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(file, "File", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(line, "Line", int) 

using namespace std::literals;
namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;

//Эта конструкция установит глобальный фильтр, пропускающий только сообщения с уровнем не ниже info
void InitBoostLogFilter() {
    logging::core::get()->set_filter(
        logging::trivial::severity >= logging::trivial::info
    );
} 

// Функция принимает параметр типа record_view, содержащий полную информацию о сообщении,
// и параметр типа formatting_ostream — поток, в который нужно вывести итоговый текст.
void MyFormatter(logging::record_view const& rec, logging::formatting_ostream& strm) {
    // Выводить LineID стало проще.
    strm << rec[line_id] << ": ";

    // Момент времени приходится вручную конвертировать в строку.
    // Для получения истинного значения атрибута нужно добавить
    // разыменование. 
    auto ts = *rec[timestamp];
    strm << to_iso_extended_string(ts) << ": ";

    // Выводим уровень, заключая его в угловые скобки.
    strm << "<" << rec[logging::trivial::severity] << "> ";

    // Выводим само сообщение.
    strm << rec[logging::expressions::smessage];
} 

void FileConsoleLogging() {
        //logging::add_file_log("sample.log"); 

    logging::add_common_attributes();// функция позволяет выводить следующие плейсхолдеры:
    // %LineID% — порядковый номер сообщения,
    // %TimeStamp% — момент времени,
    // %ProcessID% — идентификатор процесса,
    // %ThreadID% — идентификатор потока.
    logging::add_file_log( 
        // Boost.Log поддерживает ротацию как по дате, так и по размеру
        // В имя файла нужно добавить плейсхолдер %N
        // Boost.Log будет записывать файлы логов: sample_0.log, sample_1.log
        keywords::file_name = "sample_%N.log",
        keywords::format = "[%TimeStamp%]: %Message%", // добавим вывод момента времени
        keywords::open_mode = std::ios_base::app | std::ios_base::out, // чтобы файл не обнулялся при перезапуске
        // ротируем по достижению размера 10 мегабайт
        keywords::rotation_size = 10 * 1024 * 1024,
        // ротируем ежедневно в полдень
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0)
    ); 

    logging::add_console_log( 
        // Логично использовать поток std::clog, который специально предназначен для логирования:
        // Но фактически std::clog посылает все данные в std::cerr
        std::clog,
        keywords::format = "[%TimeStamp%]: %Message%",
        keywords::auto_flush = true
    ); 
}

int main() {
    logging::add_common_attributes();
    logging::add_file_log(
        keywords::file_name = "sample.log",
        keywords::format = &MyFormatter
    );


    InitBoostLogFilter();

    //[системное время] [идентификатор треда] [уровень] Сообщение 
    BOOST_LOG_TRIVIAL(trace) << "Сообщение уровня trace"sv;
    BOOST_LOG_TRIVIAL(debug) << "Сообщение уровня debug"sv;
    BOOST_LOG_TRIVIAL(info) << "Сообщение уровня info"sv;
    BOOST_LOG_TRIVIAL(warning) << "Сообщение уровня warning"sv;
    BOOST_LOG_TRIVIAL(error) << "Сообщение уровня error"sv;
    BOOST_LOG_TRIVIAL(fatal) << "Сообщение уровня fatal"sv;
    BOOST_LOG_TRIVIAL(info) 
        << logging::add_value(file, __FILE__) 
        << logging::add_value(line, __LINE__) 
        << "Something happend"sv; 
}