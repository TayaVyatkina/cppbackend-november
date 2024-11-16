#pragma once
#include "tagged.h"
#include "typedef.h"

#include <chrono>
#include <string>
#include <unordered_map>

namespace model {


    class Dog {
        inline static size_t cntMaxId = 0;

    public:
        using Id = util::Tagged<size_t, Dog>;

        Dog(std::string name) :
            id_(Id{ Dog::cntMaxId++ }),
            name_(name) {};

        Dog(Id id, std::string name) :
            id_(id),
            name_(name) {};


        Dog(const Dog& other) = default;
        Dog(Dog&& other) = default;
        Dog& operator = (const Dog& other) = default;
        Dog& operator = (Dog&& other) = default;
        virtual ~Dog() = default;

        const Id& GetId() const;                  
        const std::string& GetName() const;        

        const Direction GetDirection() const;       
        void SetDirection(Direction direction);     
        
        const Position& GetPosition() const;       
        void SetPosition(Position position);    
        
        const Speed& GetSpeed() const;          
        void SetSpeed(Speed velocity);           
        
        void Move(Direction direction, double speed);
        Position CalculateNewPosition(const std::chrono::milliseconds& diff_time);     

    private:
        Id id_;                                    
        std::string name_;                          
        Direction direction_{ Direction::NORTH };   
        Position position_{ 0.0, 0.0 };            
        Speed speed_{ 0.0, 0.0 };                  
    };

}