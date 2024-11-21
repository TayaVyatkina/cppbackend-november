#include "game_session.h"

namespace app {

    const GameSession::Id& GameSession::GetId() const noexcept {
        return id_;
    }

    const std::shared_ptr<model::Map> GameSession::GetMap() {
        return map_;
    };

    std::shared_ptr<GameSession::SessionStrand> GameSession::GetStrand() {
        return strand_;
    };

    GameSession::DogsId& GameSession::GetDogs()  {
        return dogs_;
    }

    void GameSession::GenerateLoot(const TimeInterval& interval) {

        auto tmpObjLoot = lootGenerator_.Generate(interval, lostObjects_.size(), dogs_.size());

        for (size_t i = 0; i < tmpObjLoot; ++i) {
            GenerateLostObject();
        }
    };

    void GameSession::PutLootInRndPosition(std::shared_ptr<model::LostObject> loot) {
        loot->SetPosition(map_->GenerateRndPosition());
    };

    void GameSession::GenerateLostObject() {
        auto tmpLostObj = std::make_shared<model::LostObject>();

        tmpLostObj->SetType(randomgen::RandomLootType(0, map_->GetSizeLootTypes() - 1));
        PutLootInRndPosition(tmpLostObj);
        tmpLostObj->SetValue(map_->GetLootTypeViaIdx(tmpLostObj->GetType()).value);
        lostObjects_[tmpLostObj->GetId()] = tmpLostObj;
    };

    const GameSession::lostObjectsId& GameSession::GetLostObj() {
        return lostObjects_;
    }

    void GameSession::StartGame() {

        if (gameRefreshPeriod_.count() != 0) {
            gameTicker_ = std::make_shared<tickerTime::Ticker>(
                strand_,
                gameRefreshPeriod_,
                std::bind(&GameSession::UpdateGameState, this, std::placeholders::_1) 
            );
            gameTicker_->Start();
        }
    };

    void GameSession::UpdateGameState(const TimeInterval& period) {
        bool noMove = true;
        for (auto [id, dog] : dogs_) {
            auto [new_pos, new_speed] = map_->GetMove(
                dog->GetPosition(),
                dog->CalculateNewPosition(period),
                dog->GetSpeed()
            );
            if (dog->GetPosition() != new_pos) {
                noMove = false;
            }
            dog->SetPosition(new_pos);
            dog->SetSpeed(new_speed);
        }

        if (!noMove) {
            FindAndReturnLoot();
        }

        GenerateLoot(period);
        
    }

    void GameSession::FindAndReturnLoot() {
        std::vector< std::shared_ptr<collision_detector::Item> > tmpItems;          
        std::vector< std::shared_ptr<model::Dog> > tmpDogs;                      

        for (auto [id, obj] : lostObjects_) {   
            tmpItems.push_back(obj);
        }

        for (auto [id, dog] : dogs_) {
            tmpDogs.push_back(dog);
        }

        for (auto office : map_->GetOffices()) {
            tmpItems.push_back(std::make_shared<model::Office>(office));
        }

        model::DogGather dogGather(std::move(tmpItems), std::move(tmpDogs));      

        auto foundEvents = collision_detector::FindGatherEvents(dogGather);

        if (!foundEvents.empty()) {                                                 
            for (const collision_detector::GatheringEvent& event : foundEvents) {   
                PickUpLoot(dogGather, event.item_id, event.gatherer_id);            
                ReturnLootInOffice(dogGather, event.item_id, event.gatherer_id);    
            }
            
        }

    }

    void GameSession::PickUpLoot(const model::DogGather& dogGather, size_t itemID, size_t gathererID) {

        if (dogGather.GetItem(itemID).GetType()) {          
            auto dog = dogs_[dogGather.GetDogIdFromIdx(gathererID)];
            auto lostObjId = dogGather.GetLostObj(itemID)->GetId();

            if (dog->CheckFullBag() || lostObjects_.contains(lostObjId))return;   
            
            dog->PickUpLoot(lostObjects_.at(lostObjId));
            auto pickedUpObjId = [id = lostObjId](const auto& item) {            
                auto const& [key, value] = item;
                return key == id; 
                };
            std::erase_if(lostObjects_, pickedUpObjId);                          
        }
    }

    void GameSession::ReturnLootInOffice(const model::DogGather& dogGather, size_t itemID, size_t gathererID) {
        if (!dogGather.GetItem(itemID).GetType()) {
            auto dog = dogs_[dogGather.GetDogIdFromIdx(gathererID)];
            if (dog->CheckEmptyBag()) {
                return;  
            }
            dog->ReturnLootInOffice();
        }
    }

    void GameSession::SetLostObject(model::LostObject obj) {
        auto lost_obj = std::make_shared<model::LostObject>(std::move(obj));
        lostObjects_[lost_obj->GetId()] = lost_obj;
    }

    void GameSession::SetDog(std::shared_ptr<model::Dog> dog) {
        dogs_[dog->GetId()] = dog;
    }

}