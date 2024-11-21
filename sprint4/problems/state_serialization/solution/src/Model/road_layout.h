#pragma once
#include "road.h"
#include "typedef.h"
#include "utils.h"

#include <map>
#include <optional>
#include <tuple>
#include <unordered_set>

namespace model {

	class RoadLayout {					
	public:
		using Roads = std::vector<Road>;

		RoadLayout() = default;
		RoadLayout(const RoadLayout& other);
		RoadLayout(RoadLayout&& other) noexcept;
		RoadLayout& operator = (const RoadLayout& other);
		RoadLayout& operator = (RoadLayout&& other);
		virtual ~RoadLayout() = default;

		void AddRoad(const Road& road);										

		const Roads& GetRoads() const noexcept;									


		std::tuple<geom::Point2D, Speed> GetValidMove(const geom::Point2D& current_pos,
			const geom::Point2D& target_pos,
			const Speed& current_spd);

		geom::Point2D GenerateRndPosition() const;

	private:

		struct MapCoord {
			int64_t x;
			int64_t y;

			bool operator == (const MapCoord& other) const {
				return (x == other.x) && (y == other.y);
			};


		};

		using MatrixMap = std::map< int64_t, std::map<int64_t, std::unordered_set<size_t> > >;
		MatrixMap matrixMap_;																		
		Roads roads_;																				


		std::optional<const MapCoord> GetDestinationRoadsOfRoute(
			std::optional<const MapCoord> start,
			std::optional<const MapCoord> end,
			const Speed& current_spd);

		std::optional<const MapCoord> GetCoordinatesOfPosition(const geom::Point2D& pos);

		const Direction SpeedToDirection(const Speed& speed);

		const std::unordered_map<Direction, geom::Point2D> MatrixCoordinateToPosition(const MapCoord& coord,
			const geom::Point2D& target_pos);

		bool IsCrossedSets(const std::unordered_set<size_t>& lhs,
			const std::unordered_set<size_t>& rhs);

		bool IsValidCoordinates(const MapCoord& coord);

		const geom::Point2D GetFarthestPoinOfRoute(const MapCoord& roads_coord,
			const geom::Point2D& current_pos,
			const Speed& current_spd);

		bool IsValidPosition(const std::unordered_set<size_t>& roads, const geom::Point2D& pos);

		bool IsValidPositionOnRoad(const Road& road, const geom::Point2D& pos);

		void CopyAll(const Roads& roads);



	};

}