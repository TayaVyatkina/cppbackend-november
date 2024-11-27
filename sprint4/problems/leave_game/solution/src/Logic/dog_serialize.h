#pragma once 
#include <algorithm>
#include <boost/serialization/vector.hpp>

#include "dog.h"
#include "lo_serialize.h"
#include "typedef.h"

namespace serialization {

    namespace ranges = std::ranges;

    class DogRepr {
    public:
        DogRepr() = default;
        explicit DogRepr(const model::Dog& dog)
            :id_(*dog.GetId()),
            name_(dog.GetName()),
            direction_(dog.GetDirection()),
            position_(dog.GetPosition()),
            bagSize_(dog.GetBagSize())
        {
            auto ConvertLostObjToSerializedLostObj = [](std::shared_ptr<model::LostObject> obj)->LostObjRepr {
                return *obj;
                };
            ranges::transform(dog.GetBag(), std::back_inserter(bag_), ConvertLostObjToSerializedLostObj);

        };

        [[nodiscard]] model::Dog Restore() const;

        template <typename Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
            ar& id_;
            ar& name_;
            ar& direction_;
            ar& position_;
            ar& bag_;
            ar& bagSize_;
            ar& score_;
        }
    private:
        size_t id_;
        std::string name_;
        model::Direction direction_;
        geom::Point2D position_{ 0.0, 0.0 };
        std::vector<LostObjRepr> bag_;
        size_t bagSize_ = 0;
        size_t score_ = 0;
    };
}