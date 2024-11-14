#pragma once
#include "game.h"
#include "player.h"
#include "tokens.h"
#include "tagged.h"
#include "ticker.h"

#include <vector>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <functional>

namespace app {

    namespace net = boost::asio;

    class Application {
    public:
        using StrandApp = net::strand<net::io_context::executor_type>;

        Application(model::Game game, size_t tick_period, bool randomize_pos, net::io_context& ioc) :
            game_(std::move(game)),
            tickPeriod_{ tick_period },
            randomizePosition_{ randomize_pos },
            ioc_(ioc),
            strand_(std::make_shared<StrandApp>(net::make_strand(ioc))) {
     
        }; 
        Application(const Application& other) = delete;
        Application(Application&& other) = delete;
        Application& operator = (const Application& other) = delete;
        Application& operator = (Application&& other) = delete;

        virtual ~Application() = default;

        const model::Game::Maps& ListMap() const noexcept;                                                  //Выдать список карт
        const std::shared_ptr<model::Map> FindMap(const model::Map::Id& id) const noexcept;                 //Найти карту
        std::tuple<auth::Token, Player::Id> JoinGame(const std::string& name, const model::Map::Id& id);    //Залогинить игорька
        const std::vector< std::weak_ptr<Player> >& GetPlayersFromSession(auth::Token token);               //Посмотреть сколько играют
        bool CheckPlayerByToken(auth::Token token);                                                         //Чекнуть, если такой в игре
        void MovePlayer(const auth::Token& token, model::Direction direction);                              //Передвижение игрока
        std::shared_ptr<StrandApp> GetStrand();                                                             //Геттер, на всякий случай...
        bool CheckTimeManage() const;                                                                       //Как управляем временем, вручную или нет
        void UpdateGameState(const std::chrono::milliseconds& time);                                        //апдейтим состояние игры
        void AddGameSession(std::shared_ptr<GameSession> session);                                          //Добавить сессию
        std::shared_ptr<GameSession> GetGameSessionByMapId(const model::Map::Id& id) const noexcept;              //Геттер сессии по айди
        std::shared_ptr<GameSession> GetGameSessionByToken(const auth::Token& token) const noexcept;           //Геттер сессии по токену
    private:
        using SessionIdHasher = util::TaggedHasher<GameSession::Id>;
        using SessionIdToIndex = std::unordered_map<GameSession::Id, std::vector< std::weak_ptr<Player> >, SessionIdHasher>;

        using MapIdHasher = util::TaggedHasher<model::Map::Id>;
        using MapIdToSessionIndex = std::unordered_map<model::Map::Id, size_t, MapIdHasher>;

        using TokenToSessionIndex = std::unordered_map<auth::Token, std::shared_ptr<GameSession>, auth::TokenHasher>;

        model::Game game_;                                                                                  //Объект игры
        std::chrono::milliseconds tickPeriod_;                                                              //Период обновления
        bool randomizePosition_;                                                                            //Флаг того ставитьли собак рандомно или в {0 0} 
        std::vector< std::shared_ptr<Player> > players_;                                                    //Вектор игорьков
        SessionIdToIndex sessionID_;                                                                        //ID сессии
        auth::PlayerTokens playerTokens_;
        net::io_context& ioc_;
        std::shared_ptr<StrandApp> strand_;                                                                 //На всякий случай...
        std::shared_ptr<tickerTime::Ticker> ticker_;                                                        //Тикер
        std::vector< std::shared_ptr<GameSession> > sessions_;                                              //из гейм сессии перенос, после рефактора
        MapIdToSessionIndex mapIdToSessionIndex_;                                                           //из гейм сессии перенос, после рефактора
        TokenToSessionIndex tokenToSessionIndex_;                                                           //мапа для соотношения токенов с сессией


        std::shared_ptr<Player> CreatePlayer(const std::string& player_name);                               //Создать игорька
        void BindPlayerInSession(std::shared_ptr<Player> player,                                            //Забиндить игорька в сессию
            std::shared_ptr<GameSession> session);
    };

}