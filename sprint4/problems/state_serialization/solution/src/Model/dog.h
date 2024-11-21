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

    public:
        using Id = util::Tagged<size_t, Dog>;

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
        void SetSpeed(Speed velocity);          
        
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
    };

}