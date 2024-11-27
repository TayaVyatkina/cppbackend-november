#pragma once
#include "collision_detector.h"
#include "constdef.h"
#include "lost_obj.h"
#include "tagged.h"
#include "typedef.h"
#include "geom.h"

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>

namespace model {

    class Dog {
        inline static size_t cntMaxId = 0;
        static inline std::chrono::seconds maxDownTime_{ MINUTE_IN_S };
        
    public:
        using Id = util::Tagged<size_t, Dog>;
        static void SetMaxStoppedTime(size_t time);

        Dog() = default;
        Dog(std::string name, size_t bagSize) :
            id_(Id{ Dog::cntMaxId++ }),
            name_(name),
            bagSize_(bagSize)
        {};

        Dog(Id id, std::string name, size_t bagSize) :
            id_(id),
            name_(name),
            bagSize_(bagSize) 
        {};

        Dog(const Dog& other) = default;
        Dog(Dog&& other) = default;
        Dog& operator = (const Dog& other) = default;
        Dog& operator = (Dog&& other) = default;
        virtual ~Dog() = default;

        const Id& GetId() const;
        const std::string& GetName() const;

        const Direction GetDirection() const;
        void SetDirection(Direction direction);
        
        const geom::Point2D& GetPosition() const;
        void SetPosition(geom::Point2D position);
        
        const Speed& GetSpeed() const;
        void SetSpeed(Speed speed);
        
        void Move(Direction direction, double speed);
        geom::Point2D CalculateNewPosition(const std::chrono::milliseconds& diffTime);

        const collision_detector::Gatherer& GetGatherer() const;

        bool CheckFullBag() const;
        bool CheckEmptyBag() const;
        void PickUpLoot(std::shared_ptr<LostObject> obj);
        void ReturnLootInOffice();
        const std::vector<std::shared_ptr<LostObject>>& GetBag() const;
        const size_t GetScore() const;
        const size_t GetBagSize() const;
        std::chrono::seconds GetOnlineTime();
        bool isDogSubjToDelete();
        void AccumulateOnlineTime(std::chrono::milliseconds time);
        void AccumulateDownTime(std::chrono::milliseconds time);
        DogActivity GetDogActivity();
    private:
        Id id_;
        std::string name_;
        Direction direction_{ Direction::NORTH };
        geom::Point2D position_{ 0.0, 0.0 };
        Speed speed_{ 0.0, 0.0 };

        std::vector<std::shared_ptr<LostObject>> bag_;
        size_t bagSize_ = 0;

        collision_detector::Gatherer gatherer_{ {0.0, 0.0}, {0.0, 0.0}, DOG_WIDTH};
        size_t score_ = 0;
        void AccumulateScore(size_t score);


        DogActivity activity_;
        std::chrono::milliseconds onlineTime_{ 0 };
        std::chrono::milliseconds downTime_{ 0 };
        

    };

}