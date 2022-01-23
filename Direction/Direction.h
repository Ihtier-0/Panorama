#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <QPoint>

namespace Direction {
enum Direction {
  North,
  NorthEast,
  East,
  SouthEast,
  South,
  SouthWest,
  West,
  NorthWest
};

QPoint toPoint(const Direction &direction);
QVector<Direction> all();
} // namespace Direction

#endif // DIRECTIONS_H
