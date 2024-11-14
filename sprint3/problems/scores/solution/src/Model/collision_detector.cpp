#include "collision_detector.h"

#include <cassert>

namespace collision_detector {

CollectionResult TryCollectPoint(geom::Point2D a, geom::Point2D b, geom::Point2D c) {

    assert(b.x != a.x || b.y != a.y);
    const double u_x = c.x - a.x;
    const double u_y = c.y - a.y;
    const double v_x = b.x - a.x;
    const double v_y = b.y - a.y;
    const double u_dot_v = u_x * v_x + u_y * v_y;
    const double u_len2 = u_x * u_x + u_y * u_y;
    const double v_len2 = v_x * v_x + v_y * v_y;
    const double proj_ratio = u_dot_v / v_len2;
    const double sq_distance = u_len2 - (u_dot_v * u_dot_v) / v_len2;

    return CollectionResult(sq_distance, proj_ratio);
}

using Events = std::vector<GatheringEvent>;

Events FindGatherEvents(const ItemGathererProvider& provider) {
    Events tmpRes;

    for (size_t i = 0; i < provider.GatherersCount(); i++) {
        for (size_t j = 0; j < provider.ItemsCount(); j++) {
            collision_detector::Gatherer gath = provider.GetGatherer(i);
            collision_detector::Item item = provider.GetItem(j);
            
            CollectionResult collRes = TryCollectPoint(gath.start_pos, gath.end_pos, item.GetPosition());

            if (collRes.IsCollected(item.GetWidth() + gath.width)) {
                tmpRes.push_back({ j, i, collRes.sq_distance, collRes.proj_ratio });
            }
        }
    }


    sort(tmpRes.begin(), tmpRes.end(),
        [](const GatheringEvent& lhs, const GatheringEvent& rhs) {
        return lhs.time < rhs.time;
    });
    return tmpRes;
}


}