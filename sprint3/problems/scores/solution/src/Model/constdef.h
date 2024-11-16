#pragma once
#include <string>

namespace model {

	const std::string DEFAULT_DOG_SPEED = "defaultDogSpeed";
	const std::string DEFAULT_BAG_CAPACITY = "defaultBagCapacity";

	const std::string MAPS = "maps";
	const std::string MAP_ID = "id";
	const std::string MAP_NAME = "name";
	const std::string MAP_DOG_SPEED = "dogSpeed";

	const std::string ROADS = "roads";
	const std::string ROAD_XO = "x0";
	const std::string ROAD_YO = "y0";
	const std::string ROAD_X1 = "x1";
	const std::string ROAD_Y1 = "y1";

	const std::string BUILDINGS = "buildings";
	const std::string BUILDING_X = "x";
	const std::string BUILDING_Y = "y";
	const std::string BUILDING_WIDTH = "w";
	const std::string BUILDING_HEIGHT = "h";

	const std::string OFFICES = "offices";
	const std::string OFFICE_ID = "id";
	const std::string OFFICE_X = "x";
	const std::string OFFICE_Y = "y";
	const std::string OFFICE_OFFSET_X = "offsetX";
	const std::string OFFICE_OFFSET_Y = "offsetY";

	const std::string LOOT_GENERATOR_CONFIG = "lootGeneratorConfig";
	const std::string LOOT_GENERATOR_PERIOD = "period";
	const std::string LOOT_GENERATOR_PROBABILITY = "probability";

	const std::string LOOT_TYPE = "lootTypes";
	const std::string LOOT_TYPE_NAME = "name";
	const std::string LOOT_TYPE_FILE = "file";
	const std::string LOOT_TYPE_TYPE = "type";
	const std::string LOOT_TYPE_ROTATION = "rotation";
	const std::string LOOT_TYPE_COLOR = "color";
	const std::string LOOT_TYPE_SCALE = "scale";
	const std::string LOOT_TYPE_VALUE = "value";

	const double CONVERT_MS_TO_S = 1000.0;
	const double DOG_SPEED_INIT = 1.0;
	const size_t DOG_BAG_SIZE_INIT = 3;

	const double DOG_WIDTH = 0.6;
	const double LOOT_WIDTH = 0.0;
	const double BASE_WIDTH = 0.5;

}