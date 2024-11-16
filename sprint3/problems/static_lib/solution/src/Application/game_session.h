#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include "map.h"
#include "dog.h"
#include "tagged.h"
#include "loot_generator.h"
#include "lost_obj.h"
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

        GameSession(std::shared_ptr<model::Map> map, const model::LootGenCfg cfg, net::io_context& ioc) :
            map_(map),
            strand_(std::make_shared<SessionStrand>(net::make_strand(ioc))),
            id_(*(map->GetId())),
            lootGenerator_(TimeInterval(static_cast<uint64_t>(cfg.period)), cfg.probability)
       
        {
            lootTicker_ = std::make_shared<tickerTime::Ticker>(
                strand_,
                lootGenerator_.GetBaseInterval(),
                std::bind(&GameSession::GenerateLoot, this, std::placeholders::_1) //https://en.cppreference.com/w/cpp/utility/functional/bind
                );
            lootTicker_->Start();
      
        };


        const Id& GetId() const noexcept;
        const std::shared_ptr<model::Map> GetMap();
        std::shared_ptr<SessionStrand> GetStrand();
        DogsId& GetDogs();
        const lostObjectsId& GetLostObj();

    private:
        std::shared_ptr<model::Map> map_;                                       
        std::shared_ptr<SessionStrand> strand_;                                 
        Id id_;                                                                 
        loot_gen::LootGenerator lootGenerator_;                                
        DogsId dogs_;                                                          
        lostObjectsId lostObjects_;                                           
        std::shared_ptr<tickerTime::Ticker> lootTicker_;                       

        void GenerateLoot(const TimeInterval& interval);                       
        void GenerateLostObject();                                              
        void PutLootInRndPosition(std::shared_ptr<model::LostObject> loot);     
    };

}