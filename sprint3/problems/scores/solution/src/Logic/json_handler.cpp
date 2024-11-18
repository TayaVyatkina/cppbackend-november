#include "json_handler.h"
#include "map.h"
#include <map>
#include <sstream>
#include <boost/json/array.hpp>
#include <boost/json.hpp>

namespace jsonOperation {

    std::string GameToJson(const model::Game::Maps& game) {
        boost::json::array mapsArr;
        for (auto map : game) {
            boost::json::value item = { {model::MAP_ID, *(map->GetId())},
                                        {model::MAP_NAME, map->GetName()} };
            mapsArr.push_back(item);
        }
        return boost::json::serialize(mapsArr);
    }

    std::string MapToJson(const model::Map& map) {                                              
        return boost::json::serialize(boost::json::value_from(map));
    }

    std::string MapNotFound() {
        boost::json::value msg = { {RESPONSE_CODE, "mapNotFound"},
                                    {RESPONSE_MESSAGE, "Map not found"} };
        return boost::json::serialize(msg);
    };

    std::string BadRequest() {
        boost::json::value msg = { {RESPONSE_CODE, "badRequest"},
                                    {RESPONSE_MESSAGE, "Bad request"} };
        return boost::json::serialize(msg);
    };

    std::string PageNotFound() {
        boost::json::value msg = { {RESPONSE_CODE, "pageNotFound"},
                                    {RESPONSE_MESSAGE, "Page not found"} };
        return boost::json::serialize(msg);
    };

    std::string JoinToGameInvalidArgument() {
        boost::json::value msg = { {RESPONSE_CODE, "invalidArgument"},
                            {RESPONSE_MESSAGE, "Join game request parse error"} };
        return boost::json::serialize(msg);
    };

    std::string JoinToGameMapNotFound() {
        boost::json::value msg = { {RESPONSE_CODE, "mapNotFound"},
                            {RESPONSE_MESSAGE, "Map not found"} };
        return boost::json::serialize(msg);
    };

    std::string JoinToGameEmptyPlayerName() {
        boost::json::value msg = { {RESPONSE_CODE, "invalidArgument"},
                            {RESPONSE_MESSAGE, "Invalid name"} };
        return boost::json::serialize(msg);
    };

    std::string PlayersListOnMap(const std::vector< std::weak_ptr<app::Player> >& players) {
        boost::json::value jv;
        boost::json::object& obj = jv.emplace_object();
        for (auto item : players) {
            auto player = item.lock();
            std::stringstream ss;
            ss << *(player->GetId());
            boost::json::value jv_item = { {RESPONSE_NAME, player->GetName()} };
            obj[ss.str()] = jv_item;
        }
        return boost::json::serialize(jv);
    };

    std::string InvalidMethod() {
        boost::json::value msg = { {RESPONSE_CODE, "invalidMethod"},
                            {RESPONSE_MESSAGE, "Invalid method"} };
        return boost::json::serialize(msg);
    };

    std::string EmptyAuthorization() {
        boost::json::value msg = { {RESPONSE_CODE, "invalidToken"},
                            {RESPONSE_MESSAGE, "Authorization header is missing"} };
        return boost::json::serialize(msg);
    };

    std::string UnknownToken() {
        boost::json::value msg = { {RESPONSE_CODE, "unknownToken"},
                            {RESPONSE_MESSAGE, "Player token has not been found"} };
        return boost::json::serialize(msg);
    };

    std::string JoinToGame(const std::string& token, size_t player_id) {
        boost::json::value msg = { {RESPONSE_AUTHTOKEN, token},
                            {RESPONSE_PLAYERID, player_id} };
        return boost::json::serialize(msg);
    };

    std::string OnlyPostMethodAllowed() {
        boost::json::value msg = { {RESPONSE_CODE, "invalidMethod"},
                            {RESPONSE_MESSAGE, "Only POST method is expected"} };
        return boost::json::serialize(msg);
    };

    std::optional< std::tuple<std::string, model::Map::Id> > ParseJoinToGame(const std::string& msg) {
        try {
            boost::json::value jv = boost::json::parse(msg);
            std::string player_name = boost::json::value_to<std::string>(jv.as_object().at(REQUEST_NAME));
            model::Map::Id map_id{ boost::json::value_to<std::string>(jv.as_object().at(REQUEST_MAPID)) };
            return std::tie(player_name, map_id);
        }
        catch (...) {
            std::cerr << "json parsing error" << std::endl;
            return std::nullopt;
        }
    };

    std::string GameState(const std::vector< std::weak_ptr<app::Player> >& players, const app::GameSession::lostObjectsId lostObjects) {
        boost::json::value jv;
        boost::json::object objPLayers;
        boost::json::object objLost;
        boost::json::object tmpRes;
        for (auto item : players) {
            auto player = item.lock();
            auto dog = player->GetDog();
            std::stringstream ss;
            ss << *(player->GetId());
            boost::json::array pos = { dog->GetPosition().x, dog->GetPosition().y };
            boost::json::array speed = { dog->GetSpeed().vx, dog->GetSpeed().vy };
            boost::json::array bag;

            for (auto obj: dog->GetBag()) {
                bag.push_back({ {RESPONSE_BAG_ITEM_ID, *(obj->GetId())},
                                {RESPONSE_BAG_ITEM_TYPE, obj->GetType()} });
            }
            boost::json::value jv_item = {  {RESPONSE_DOG_POSITION, pos},
                                            {RESPONSE_DOG_SPEED, speed},
                                            {RESPONSE_DOG_DIRECTION, model::DIRECTION_TO_JSON.at(dog->GetDirection())},
                                            {RESPONSE_DOG_BAG, bag},
                                            {RESPONSE_DOG_SCORE, dog->GetScore()} };
            objPLayers[ss.str()] = jv_item;
        }
        tmpRes[RESPONSE_PLAYERS] = objPLayers;
        
        for (auto [id, tmpLo] : lostObjects) {
                std::stringstream ss;
                ss << *id;
                boost::json::value tmpLoot = {
                    {RESPONSE_LOST_OBJ_TYPE, boost::json::value_from(tmpLo->GetType())},
                    {RESPONSE_LOST_OBJ_POSITION, boost::json::array{tmpLo->GetPosition().x, tmpLo->GetPosition().y}}
                };
                objLost[ss.str()] = tmpLoot;
        }
        tmpRes[RESPONSE_LOST_OBJECTS] = objLost;

        jv.emplace_object() = tmpRes;
        return boost::json::serialize(jv);
    };

    std::string PlayerAction() {
        boost::json::value msg = { {} };
        return boost::json::serialize(msg);
    };

    std::string PlayerInvalidAction() {
        boost::json::value msg = { {RESPONSE_CODE, "invalidArgument"},
                            {RESPONSE_MESSAGE, "Failed to parse action"} };
        return boost::json::serialize(msg);
    };

    std::string InvalidContentType() {
        boost::json::value msg = { {RESPONSE_CODE, "invalidArgument"},
                            {RESPONSE_MESSAGE, "Invalid content type"} };
        return boost::json::serialize(msg);
    };

    std::optional<std::string> ParsePlayerActionRequest(const std::string& msg) {
        try {
            boost::json::value jv = boost::json::parse(msg);
            std::string direction = boost::json::value_to<std::string>(jv.as_object().at(REQUEST_MOVE));
            return direction;
        }
        catch (...) {
            std::cerr << "parsing player action error" << std::endl;
            return std::nullopt;
        }
    };

    std::string InvalidEndpoint() {
        boost::json::value msg = { {RESPONSE_CODE, "badRequest"},
                            {RESPONSE_MESSAGE, "Invalid endpoint"} };
        return  boost::json::serialize(msg);
    };

    std::string SetDeltaTime() {
        boost::json::object msg = {};
        return boost::json::serialize(msg);
    };

    std::string InvalidDeltaTime() {
        boost::json::value msg = { {RESPONSE_CODE, "invalidArgument"},
                            {RESPONSE_MESSAGE, "Failed to parse tick request JSON"} };
        return boost::json::serialize(msg);
    };

    std::optional<int> ParseSetDeltaTimeRequest(const std::string& msg) {
        try {
            boost::json::value jv = boost::json::parse(msg);
            if (!jv.as_object().at(REQUEST_TIME_DELTA).is_int64()) {
                return std::nullopt;
            }
            int time_delta = boost::json::value_to<int>(jv.as_object().at(REQUEST_TIME_DELTA));
            return time_delta;
        }
        catch (...) {
            std::cerr << "setting delta time request error" << std::endl;
            return std::nullopt;
        }
    };

}