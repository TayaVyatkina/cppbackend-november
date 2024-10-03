#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>

using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
    auto GetTime() const {
        std::lock_guard g(lock_time_);
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        // C++20 позволяет напрямую выводить time_point в поток,
        // но на данный момент эта возможность есть не во всех популярных реализациях
        // стандартной библиотеки, поэтому воспользуемся функцией 
        // to_time_t для форматирования момента времени.
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);

        // %F используется для даты, а %T для времени. Оба значения будут 
        // выведены в формате ISO 8601. Полное описание всех плейсхолдеров:
        // https://en.cppreference.com/w/cpp/io/manip/put_time
        return std::put_time(std::localtime(&t_c), "%F %T");
    }

    // Для имени файла возьмите дату с форматом "%Y_%m_%d"
    std::string GetFileTimeStamp(const std::chrono::_V2::system_clock::time_point now) const{
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t_c), "%Y_%m_%d");
        return ss.str();
    }

    // конструктор теперь приватный
    Logger() = default;
    // убираем конструктор копирования
    Logger(const Logger&) = delete;

public:
    // получение ссылки на единственный объект
    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    
    // Функция для вывода одного и более значений
    template <typename T0, typename... Ts>
    void LogImpl(std::ostream& out, const T0& v0, const Ts&... vs) {
        using namespace std::literals;
        //std::lock_guard lg(m_);
        out << GetTimeStamp() << ": "sv << v0 << std::endl;
        // Выводим остальные параметры, если они остались
        if constexpr (sizeof...(vs) != 0) {
            LogImpl(out, vs...);  // Рекурсивно выводим остальные параметры
        }
    }

    void CheckNameLogFile(const std::chrono::_V2::system_clock::time_point now){
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        int day_of_month = std::localtime(&t_c)->tm_mday;
        if(day_of_file_ != day_of_month){
            day_of_file_ = day_of_month;
            std::string file_name = "/var/log/sample_log_"s + GetFileTimeStamp(now) + ".log"s;
            log_file_.close(); 
            log_file_.open(file_name); //, std::ios::app
            if (!log_file_) {
                throw std::runtime_error("File not open!"s);
            }
        }
    }

    // Выведите в поток все аргументы.
    //template<class... Ts>
    template <typename... Ts>
    void Log(const Ts&... args) {
        const auto now = GetTime();
        std::stringstream strm;
        if constexpr (sizeof...(args) != 0) {
            LogImpl(strm, args...);
        }
        std::lock_guard g(lock_file_);
        CheckNameLogFile(now);
        log_file_ << strm.str() << std::endl;
        
    }
    // Установите manual_ts_. Учтите, что эта операция может выполняться
    // параллельно с выводом в поток, вам нужно предусмотреть 
    // синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts){
        std::lock_guard lg(lock_time_);
        manual_ts_ = ts;
    }

private:
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
    std::ofstream log_file_;
    int day_of_file_ = 0;
    mutable std::mutex lock_time_;
    mutable std::mutex lock_file_;
};
