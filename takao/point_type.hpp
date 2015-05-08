#ifndef POINT_TYPE
#define POINT_TYPE

// 何らかの座標を表すクラス
class SHARED_EXPORT point_type
{
    public:
        point_type() = default;
        ~point_type() = default;

        int y;
        int x;

        friend inline bool operator== (point_type const& lhs, point_type const& rhs)
        {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
        friend inline bool operator!= (point_type const& lhs, point_type const& rhs)
        {
            return lhs.x != rhs.x || lhs.y != rhs.y;
        }
        friend inline bool operator< (point_type const& lhs, point_type const& rhs)
        {
            return (lhs.x == rhs.x) ? lhs.y < rhs.y : lhs.x < rhs.x;
        }
        friend inline point_type const operator- (point_type const& lhs, point_type const& rhs)
        {
            return point_type{ lhs.x - rhs.x, lhs.y - rhs.y };
        }
        friend inline point_type const operator+ (point_type const& lhs, point_type const& rhs)
        {
            return point_type{ lhs.x + rhs.x, lhs.y + rhs.y };
        }
        friend inline std::ostream& operator<<(std::ostream& os, const point_type& point)
        {
            os << '{' << point.y << ',' << point.x << '}';
            return os;
        }

};

#endif // POINT_TYPE
