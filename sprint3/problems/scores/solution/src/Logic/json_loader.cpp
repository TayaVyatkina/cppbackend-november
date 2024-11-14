#include "json_loader.h"
#include "lost_obj.h"
#include "logger.h"
#include "typedef.h"
#include "tag_invoke_handler.h"

#include <fstream>
#include <iostream>
#include <string_view>
#include <sstream>

#include <iostream>

namespace json_loader {

    namespace json = boost::json;                                                       
    using namespace std::literals;

    boost::json::value ReadJSONFile(const std::filesystem::path& json_path) {
        std::ifstream file(json_path);
        if (!file.is_open()) {
            BOOST_LOG_TRIVIAL(error) << logger::CreateLogMessage("error"sv,
                logger::ExceptionLog(EXIT_FAILURE,
                    "Error: Can not open current file"sv, "Invalid path"sv)); 
            throw std::invalid_argument("Invalid path, can not open file");                   
        }

        std::stringstream ss;
        ss << file.rdbuf();
        boost::json::value root = boost::json::parse(ss.str());
        return root;
    };

    model::Game LoadGame(const std::filesystem::path& json_path) {

        model::Game game;
        boost::json::value jsonVal = ReadJSONFile(json_path);

        model::LootGenCfg lootGeneratorConfig = boost::json::value_to<model::LootGenCfg>(jsonVal.as_object().at(model::LOOT_GENERATOR_CONFIG));
        game.AddLootGeneratorConfig(lootGeneratorConfig);

        std::vector<model::Map> maps = boost::json::value_to< std::vector<model::Map> >(jsonVal.as_object().at(model::MAPS));
        game.AddMaps(maps);

        try {
            double dogSpeed = boost::json::value_to<double>(jsonVal.as_object().at(model::DEFAULT_DOG_SPEED));
            game.SetInitDogSpeed(dogSpeed);
        }
        catch (...) {}


        try {
            double dogBagSize = boost::json::value_to<double>(jsonVal.as_object().at(model::DEFAULT_BAG_CAPACITY));
            game.SetInitDogBagSize(dogBagSize);
        }
        catch (...) {}

        return game;
    };

} 
