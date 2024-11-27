#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/signals2/signal.hpp>

#include "map.h"
#include "dog.h"
#include "dog_gather.h"
#include "tagged.h"
#include "loot_generator.h"
#include "lost_obj.h"
#include "player_data_record.h"
#include "ticker.h"
#include "utils.h"

#include <chrono>
#include <vector>
#include <memory>
#include <unordered_map>


namespace app {

    namespace net = boost::asio;


    class GameSession : public std::enable_shared_from_this<GameSession> {
    public:
        using Id = util::Tagged<std::string, GameSession>;
        using DogsIdHasher = util::TaggedHasher<model::Dog::Id>;
        using SessionStrand = net::strand<net::io_context::executor_type>;
        using DogsId = std::unordered_map<model::Dog::Id, std::shared_ptr<model::Dog>, DogsIdHasher>;
        using lostObjIdHasher = util::TaggedHasher<model::LostObject::Id>;
        using lostObjectsId = std::unordered_map<model::LostObject::Id, std::shared_ptr<model::LostObject>, lostObjIdHasher>;
        using TimeInterval = std::chrono::milliseconds;

        GameSession(std::shared_ptr<model::Map> map, const TimeInterval& gameRefreshPeriod, const model::LootGenCfg cfg, net::io_context& ioc) :
            map_(map),
            strand_(std::make_shared<SessionStrand>(net::make_strand(ioc))),
            id_(*(map->GetId())),
            lootGenerator_(TimeInterval(static_cast<uint64_t>(cfg.period * model::CONVERT_MS_TO_S)), cfg.probability),
            gameRefreshPeriod_(gameRefreshPeriod)
        {

        };

        const Id& GetId() const noexcept;
        const std::shared_ptr<model::Map> GetMap();
        std::shared_ptr<SessionStrand> GetStrand();
        DogsId& GetDogs();
        const lostObjectsId& GetLostObj();
        void UpdateGameState(const TimeInterval& period);
        void StartGame();

        void SetLostObject(model::LostObject obj);
        std::shared_ptr<model::Dog> MakeDog(const std::string& name,
                                            const model::Map& map,
                                            bool randomize_spawn_points);
        void SetDog(std::shared_ptr<model::Dog> dog);

        void SetDeleteAFKPlayersFuntional(std::function<void(const Id&)> func);
        void SetSaveDataToPostgresFunctional(std::function<void(const std::vector<PlayerDataForPostgres>&)> func);

    private:
        std::shared_ptr<model::Map> map_;
        std::shared_ptr<SessionStrand> strand_;
        Id id_;
        loot_gen::LootGenerator lootGenerator_;
        TimeInterval gameRefreshPeriod_;
        DogsId dogs_;
        lostObjectsId lostObjects_;
        std::shared_ptr<tickerTime::Ticker> gameTicker_;
        


        void GenerateLoot(const TimeInterval& interval);
        void GenerateLostObject();
        void PutLootInRndPosition(std::shared_ptr<model::LostObject> loot);

        void FindAndReturnLoot();
        void PickUpLoot(const model::DogGather& dogGather, size_t itemID, size_t gathererID);
        void ReturnLootInOffice(const model::DogGather& dogGather, size_t itemID, size_t gathererID);

        void DeleteDogsAFKUsers();

        boost::signals2::signal<void(const Id&)> DeleteAFKPlayers_signal;       
        boost::signals2::signal<void(const std::vector<PlayerDataForPostgres>&)> SaveDataToPostgres_signal;
    };

}