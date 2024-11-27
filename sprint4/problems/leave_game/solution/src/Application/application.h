#pragma once

#include "game.h"
#include "game_session_serialize.h"
#include "player.h"
#include "postgres.h"
#include "tokens.h"
#include "tagged.h"
#include "ticker.h"
#include "typedef.h"
#include "use_cases_impl.h"

#include <vector>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <fstream>

namespace app {

    namespace net = boost::asio;

    class Application : public std::enable_shared_from_this<Application> {
    public:
        using StrandApp = net::strand<net::io_context::executor_type>;

        using TokenPlayer = std::unordered_map< auth::Token, Player::Id, auth::TokenHasher>;

        Application(model::Game game, size_t tick_period, bool randomize_pos, net::io_context& ioc, const Connection::ConnectionConfig& databaseCfg) :
            game_(std::move(game)),
            tickPeriod_{ tick_period },
            randomizePosition_{ randomize_pos },
            ioc_(ioc),
            database_{databaseCfg} {
     
        };

        Application(const Application& other) = delete;
        Application(Application&& other) = delete;
        Application& operator = (const Application& other) = delete;
        Application& operator = (Application&& other) = delete;

        virtual ~Application() = default;

        const model::Game::Maps& ListMap() const noexcept;
        const std::shared_ptr<model::Map> FindMap(const model::Map::Id& id) const noexcept;
        std::tuple<auth::Token, Player::Id> JoinGame(const std::string& name, const model::Map::Id& id);
        std::vector< std::shared_ptr<Player>> GetPlayersFromSession(auth::Token token);
        bool CheckPlayerByToken(auth::Token token);
        void MovePlayer(const auth::Token& token, model::Direction direction);
        bool CheckTimeManage() const;
        void UpdateGameState(const std::chrono::milliseconds& time);
        void AddGameSession(std::shared_ptr<GameSession> session);
        std::optional<std::shared_ptr<GameSession>>GetGameSessionByMapId(const model::Map::Id& id) const noexcept;
        std::optional<std::shared_ptr<GameSession>>GetGameSessionByToken(const auth::Token& token) const noexcept;
        void LoadGameFromSave(savegame::SavedFileParameters parameters);
        void SaveGame();
        std::optional<std::vector<app::PlayerDataForPostgres>> GetRecords(
            std::optional<size_t> offset, std::optional<size_t> limit);

    private:
        using SessionIdHasher = util::TaggedHasher<GameSession::Id>;

        using MapIdHasher = util::TaggedHasher<model::Map::Id>;
        using MapIdToSessionIndex = std::unordered_map<model::Map::Id, GameSession::Id, MapIdHasher>;

        model::Game game_;
        std::chrono::milliseconds tickPeriod_;
        bool randomizePosition_;
        net::io_context& ioc_;
        postgres::Database database_;
        postgres::UseCasesImpl use_cases_{ database_.GetDataFromImpl() };

        auth::PlayerTokens playerTokens_;
        std::unordered_map<Player::Id, std::shared_ptr<Player>, util::TaggedHasher<Player::Id>> players_;
        std::unordered_map<GameSession::Id, TokenPlayer, 
            util::TaggedHasher<GameSession::Id>> sessionToTokenPlayer_;

        std::unordered_map<GameSession::Id, std::shared_ptr<GameSession>, 
            util::TaggedHasher<GameSession::Id>> sessions_;
        MapIdToSessionIndex mapIdToSessionIndex_;

        std::shared_ptr<Player> CreatePlayer(const std::string& player_name);
        void BindPlayerInSession(std::shared_ptr<Player> player,
            std::shared_ptr<GameSession> session);

        savegame::SavedFileParameters savedParameters_;
        std::shared_ptr<tickerTime::Ticker> saveTicker_;
        void DeserializationGameState();
        std::vector<serialization::GameSessionRepr>SerializeGame();
        void SaveGamePeriodically(const std::chrono::milliseconds& time);
        void StartSaveTicker();
        
        void DeleteAFKPlayers(const GameSession::Id& input_id);
        void SavePlayerDataInDB(std::vector<PlayerDataForPostgres> data);
    };

}