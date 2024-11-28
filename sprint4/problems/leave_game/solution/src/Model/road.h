#pragma once
#include "tagged.h"
#include "dog.h"
#include "typedef.h"
#include "constdef.h"

#include <cmath>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <boost/json.hpp>

namespace model {

    class Road {
        struct HorizontalTag {
            HorizontalTag() = default;
        };

        struct VerticalTag {
            VerticalTag() = default;
        };

    public:
        constexpr static HorizontalTag HORIZONTAL{};
        constexpr static VerticalTag VERTICAL{};

        Road(HorizontalTag, Point start, Coord end_x) noexcept
            : start_{ start }
            , end_{ end_x, start.y } {
        }

        Road(VerticalTag, Point start, Coord end_y) noexcept
            : start_{ start }
            , end_{ start.x, end_y } {
        }

        bool IsHorizontal() const noexcept {
            return start_.y == end_.y;
        }

        bool IsVertical() const noexcept {
            return start_.x == end_.x;
        }

        Point GetStart() const noexcept {
            return start_;
        }

        Point GetEnd() const noexcept {
            return end_;
        }

    private:
        Point start_;
        Point end_;
    };

    void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Road& road);
    Road tag_invoke(boost::json::value_to_tag<Road>, const boost::json::value& jv);

}