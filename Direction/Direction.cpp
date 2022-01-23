#include "Direction.h"

#include <QVector>

namespace Direction {

QPoint toPoint(const Direction &direction) {
  switch (direction) {
  case Direction::North:
    return {0, 1};
  case Direction::NorthEast:
    return {1, 1};
  case Direction::East:
    return {1, 0};
  case Direction::SouthEast:
    return {1, -1};
  case Direction::South:
    return {0, -1};
  case Direction::SouthWest:
    return {-1, -1};
  case Direction::West:
    return {-1, 0};
  case Direction::NorthWest:
    return {-1, 1};
  }

  Q_UNREACHABLE();
}

QVector<Direction> all() {
  return {North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest};
}

} // namespace Direction
