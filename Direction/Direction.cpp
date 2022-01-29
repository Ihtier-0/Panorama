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

QColor toColor(const QPoint &point) {
  if (point == QPoint{0, 1}) {
    return QColorConstants::Black;
  }
  if (point == QPoint{1, 1}) {
    return QColorConstants::White;
  }
  if (point == QPoint{1, 0}) {
    return QColorConstants::Gray;
  }
  if (point == QPoint{1, -1}) {
    return QColorConstants::Red;
  }
  if (point == QPoint{0, -1}) {
    return QColorConstants::Green;
  }
  if (point == QPoint{-1, -1}) {
    return QColorConstants::Blue;
  }
  if (point == QPoint{-1, 0}) {
    return QColorConstants::Cyan;
  }
  if (point == QPoint{-1, 1}) {
    return QColorConstants::Magenta;
  }

  return QColorConstants::Yellow;
}

QColor toColor(const Direction &direction) {
  return toColor(toPoint(direction));
}

QImage toImage(const Matrix<QPoint> &matrix) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  QImage image(width, height, QImage::Format_RGB888);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      image.setPixelColor(x, y, toColor(matrix[x][y]));
    }
  }

  return image;
}

} // namespace Direction
