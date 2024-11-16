#pragma once
#include "geom.h"
#include "collision_detector.h"
#include "typedef.h"
#include "tagged.h"


namespace model {

	class LostObject : public collision_detector::Item {
	public:

		using Id = util::Tagged<size_t, LostObject>;

		LostObject(double width = LOOT_WIDTH) :
			id_(Id{ LostObject::start_id_++ }),
			Item({0, 0}, width)
		{};


		const Id& GetId() const;



	private:
		inline static size_t start_id_ = 0;		
		Id id_;									
		size_t value_{ 0 };						
	};


	struct lostObjHasher {
		size_t operator()(const LostObject& obj) const {
			size_t randomNumber = 10;
			return std::hash<size_t>{}(obj.GetType()) * randomNumber +
				std::hash<double>{}(obj.GetPosition().x) * randomNumber +
				std::hash<double>{}(obj.GetPosition().y) * randomNumber;
		}
	};
}