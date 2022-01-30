#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <QColor>
#include <QImage>
#include <QPoint>

#include "../aliases.h"

namespace Direction {
enum Direction {
  North,
  NorthEast,
  East,
  SouthEast,
  South,
  SouthWest,
  West,
  NorthWest,
  Unknown
};

QPoint toPoint(const Direction &direction);
Direction fromPoint(const QPoint &point);
QVector<Direction> all();
QColor toColor(const QPoint &point);
QColor toColor(const Direction &direction);
QImage toImage(const Matrix<QPoint> &matrix);
} // namespace Direction

#endif // DIRECTIONS_H
