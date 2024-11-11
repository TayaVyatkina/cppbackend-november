#include "application.h"
#include "dog.h"

#include <iostream>

namespace app {

    using namespace std::literals;

    const model::Game::Maps& Application::ListMap() const noexcept {
        return game_.GetMaps();
    };

    const std::shared_ptr<model::Map> Application::FindMap(const model::Map::Id& id) const noexcept {
        return game_.FindMap(id);
    };

    std::tuple<auth::Token, Player::Id> Application::JoinGame(
        const std::string& name,
        const model::Map::Id& id) {

        auto player = CreatePlayer(name);
        auto token = playerTokens_.AddPlayer(player);

        std::shared_ptr<GameSession> session = GetGameSessionByMapId(id);
        if (!session) {
            session = std::make_shared<GameSession>(game_.FindMap(id), game_.GetLootGenConfig(), ioc_);
            AddGameSession(session);
        }
        tokenToSessionIndex_[token] = session;
        BindPlayerInSession(player, session);
        return std::tie(token, player->GetId());
    };

    std::shared_ptr<Player> Application::CreatePlayer(const std::string& player_name) {
        auto player = std::make_shared<Player>(player_name);
        players_.push_back(player);
        return player;
    };

    void Application::BindPlayerInSession(std::shared_ptr<Player> player,
        std::shared_ptr<GameSession> session) {
        sessionID_[session->GetId()].push_back(player);
        player->SetGameSession(session);
        player->SetDog(player->GetName(), *(session->GetMap()), randomizePosition_);
    };

    const std::vector< std::weak_ptr<Player> >& Application::GetPlayersFromSession(auth::Token token) {
        static const std::vector< std::weak_ptr<Player> > emptyPlayerList;
        auto player = playerTokens_.FindPlayerByToken(token).lock();
        auto session_id = player->GetSessionId();
        if (!sessionID_.contains(session_id)) {
            return emptyPlayerList;
        }
        return sessionID_[session_id];
    };

    bool Application::CheckPlayerByToken(auth::Token token) {
        return !playerTokens_.FindPlayerByToken(token).expired();
    };

    void Application::UpdateGameState(const std::chrono::milliseconds& time) {
        for (auto player : players_) {
            player->MoveDog(time);
        }
    };

    std::shared_ptr<Application::StrandApp> Application::GetStrand() {
        return strand_;
    };

    void Application::MovePlayer(const auth::Token& token, model::Direction direction) {
        auto player = playerTokens_.FindPlayerByToken(token).lock();
        auto dog = player->GetDog();
        double speed = player->GetSession()->GetMap()->GetDogSpeed();
        dog->Move(direction, speed);
    };

    bool Application::CheckTimeManage() const{
        return tickPeriod_.count() == 0;
    };

    void Application::AddGameSession(std::shared_ptr<GameSession> session) {
        const size_t index = sessions_.size();
        if (auto [it, inserted] = mapIdToSessionIndex_.emplace(session->GetMap()->GetId(), index); !inserted) {
            throw std::invalid_argument("Game session with map id "s + *(session->GetMap()->GetId()) + " already exists"s);
        }
        else {
            try {
                sessions_.push_back(session);
            }
            catch (...) {
                mapIdToSessionIndex_.erase(it);
                throw;
            }
        }
    };

    std::shared_ptr<GameSession> Application::GetGameSessionByMapId(const model::Map::Id& id) const noexcept {
        if (auto it = mapIdToSessionIndex_.find(id); it != mapIdToSessionIndex_.end()) {
            return sessions_.at(it->second);
        }
        return nullptr;
    };

    std::shared_ptr<GameSession> Application::GetGameSessionByToken(const auth::Token& token) const noexcept {
        if (auto it = tokenToSessionIndex_.find(token); it != tokenToSessionIndex_.end()) {
            return it->second;
        }
        return nullptr;
    }
}