#pragma once
#include "tagged.h"
#include "game_session.h"

#include <string>
#include <vector>

namespace app {

    class Player {
        inline static size_t cntMaxId = 0;
    public:
        using Id = util::Tagged<size_t, Player>;
        Player(std::string name) :
            id_(Id{ Player::cntMaxId++ }),
            name_(name) {};
        Player(Id id, std::string name) :
            id_(id),
            name_(name) {};
        Player(const Player& other) = default;
        Player(Player&& other) = default;
        Player& operator = (const Player& other) = default;
        Player& operator = (Player&& other) = default;
        virtual ~Player() = default;


        void SetGameSession(std::shared_ptr<GameSession> session);
        void SetDog(std::shared_ptr<model::Dog> dog);

        const Id& GetId() const;
        const std::string& GetName() const;
        const GameSession::Id& GetSessionId() const;
        std::shared_ptr<GameSession> GetSession();
        std::shared_ptr<model::Dog> GetDog();

    private:

        Id id_;
        std::string name_;
        std::shared_ptr<GameSession> session_;
        std::shared_ptr<model::Dog> dog_;
    };

}