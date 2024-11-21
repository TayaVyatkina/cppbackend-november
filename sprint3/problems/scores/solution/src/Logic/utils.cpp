#include "utils.h"
#include <string>

namespace requestHandler {

    const std::string BEARER = "Bearer";
    const size_t TOKEN_SIZE = 32;
    const size_t AUTHORIZATION_NUMBER_PARTS = 2;
    const size_t BEARER_INDEX = 0;
    const size_t TOKEN_INDEX = 1;

    std::vector<std::string_view> SplitStr(std::string_view str) {
        std::vector<std::string_view> result;
        std::string delim = "/";
        if (str.empty() || str == delim) { 
            return result;
        }
        auto tmpStr = str.substr(1);
        auto start = 0U;
        auto end = tmpStr.find(delim);
        while (end != std::string::npos) {
            result.push_back(tmpStr.substr(start, end - start));
            start = end + delim.length();
            end = tmpStr.find(delim, start);
        }
        result.push_back(tmpStr.substr(start, end));
        return result;
    };

    std::string GetBearerToken(std::string_view bearer_string) {
        std::string token;
        std::vector<std::string_view> splitted;
        std::string delim = " ";
        if (bearer_string.empty() || bearer_string == delim) {
            return token;
        }
        auto start = 0U;
        auto end = bearer_string.find(delim, start);
        while (end != std::string::npos) {
            splitted.push_back(bearer_string.substr(start, end - start));
            start = end + delim.length();
            end = bearer_string.find(delim, start);
        }
        splitted.push_back(bearer_string.substr(start, end));
        if (splitted.size() != AUTHORIZATION_NUMBER_PARTS ||
            splitted[BEARER_INDEX] != BEARER ||
            splitted[TOKEN_INDEX].size() != TOKEN_SIZE) {
            return token;
        }
        return std::string(splitted[TOKEN_INDEX]);
    };

}

namespace randomgen {

    double RandomDouble(const double thl, const double thh) {
        std::random_device rd;
        std::default_random_engine eng(rd());
        std::uniform_real_distribution<double> distr(thl, thh);
        return distr(eng);
    };

    int RandomInt(const int thl, const int thh) {
        std::random_device rd;
        std::default_random_engine eng(rd());
        std::uniform_int_distribution<int> distr(thl, thh);
        return distr(eng);
    };

    size_t RandomLootType(const size_t lower, const size_t upper) {
        std::random_device rd;
        std::default_random_engine eng(rd());
        std::uniform_int_distribution<size_t> distr(lower, upper);
        return distr(eng);
    };
}