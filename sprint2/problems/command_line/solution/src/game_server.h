#pragma once

#include "json_loader.h"
#include "model_game.h"

#include <boost/asio/io_context.hpp>


namespace net = boost::asio;
namespace fs = std::filesystem;

using namespace std::literals;

class GameServer {
    GameServer() = delete;
    GameServer(const GameServer&) = delete;
    GameServer(GameServer&&) = delete;
    GameServer& operator=(const GameServer&) = delete;
    GameServer& operator=(GameServer&&) = delete;

public:
    GameServer(net::io_context& ioc, fs::path config, fs::path root) :
        ioc_(ioc),
        root_dir_(root),
        game_{json_loader::LoadGame(config)} {
        }

    const fs::path& GetRootDir() const noexcept {
        return root_dir_;
    }

    std::shared_ptr<model::Map> FindMap(const model::Map::Id& id) const noexcept {
        return game_.FindMap(id);
    }

    const std::vector<model::Map>& GetMaps() const noexcept {
        return game_.GetMaps();
    }

    std::shared_ptr<model::Player> JoinGame(model::Map::Id id, const std::string& player_name);

    std::shared_ptr<const model::Player> FindPlayer(const model::Token& token) {
        return player_list_.FindPlayer(token);
    }

    const std::unordered_map<model::Token, std::shared_ptr<model::Player>, model::TokenHasher>& GetPlayers() const noexcept {
        return player_list_.GetPlayersList();
    }

    void Tick(std::chrono::milliseconds delta) {
        game_.UpdateGame(delta.count()/1000.);
    }

    void UpdateGames() {
        game_.UpdateGame(tick_);
    }

    void SetGameServerTick(const double tick) {
        tick_ = tick;
        std::cout << "game server tick is set to " << tick_;
    }

    void SetAutoTicker() {
        auto_ticker_ = true;
    }

    void SetSpawnDogRandomPoint() {
        spawn_dog_random = true;
    }

    bool IsAutoTicker() {
        return auto_ticker_;
    }

private:
    net::io_context& ioc_;
    const fs::path root_dir_;
    model::Game game_;
    model::PlayerList player_list_;

    bool spawn_dog_random = false;
    bool auto_ticker_ = false;
    double tick_ = 0.1;

};