#include "NonMaximalSuppression.h"

Matrix<qreal> NMS(const Matrix<qreal> &probability,
                  const Matrix<QPoint> &directions, const int &tileRadius,
                  QVector<Descriptor> &descriptors) {
  Q_UNUSED(tileRadius)

  const auto width = probability.size();
  const auto height = probability[0].size();

  const auto onImage = [width, height](const int &x, const int &y) -> bool {
    return x >= 0 && y >= 0 && x < width && y < height;
  };

  Matrix<qreal> result(width, Row<qreal>(height, 0));
  Matrix<bool> used(width, Row<bool>(height, false));

  QPoint direction;
  QVector<QPair<QPoint, qreal>> border;

  int xStep;
  int yStep;
  int length;

  int maxX, maxY;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (probability[x][y] > 0 && !used[x][y]) {
        border.clear();

        used[x][y] = true;
        direction = directions[x][y];

        border.push_back({{x, y}, probability[x][y]});

        length = 1;
        while (onImage(xStep = x + direction.x() * length,
                       yStep = y + direction.y() * length) &&
               directions[xStep][yStep] == direction) {
          border.push_back({{xStep, yStep}, probability[x][y]});
          used[xStep][yStep] = true;

          ++length;
        }

        length = -1;
        while (onImage(xStep = x + direction.x() * length,
                       yStep = y + direction.y() * length) &&
               directions[xStep][yStep] == direction) {
          border.push_back({{xStep, yStep}, probability[x][y]});
          used[xStep][yStep] = true;

          --length;
        }

        auto max = *std::max_element(border.begin(), border.end(),
                                     [](const QPair<QPoint, qreal> &left,
                                        const QPair<QPoint, qreal> &right) {
                                       return left.second < right.second;
                                     });

        maxX = max.first.x();
        maxY = max.first.y();
        // просто чтобы нормально сохранить картинку
        result[maxX][maxY] = 255;

        descriptors.push_back({Direction::fromPoint(directions[maxX][maxY]),
                               probability[x][y],
                               {x, y}});
      }
    }
  }

  return result;
}

static void findZone(const int &row, const int &col,
                     const Matrix<qreal> &probability, Matrix<bool> &used,
                     const int &tileRadius,
                     QVector<QPair<QPoint, qreal>> &zone) {
  const auto width = probability.size();
  const auto height = probability[0].size();

  const auto onImage = [width, height](const int &x, const int &y) -> bool {
    return x >= 0 && y >= 0 && x < width && y < height;
  };

  for (int y = col - tileRadius; y <= col + tileRadius; ++y) {
    for (int x = row - tileRadius; x <= row + tileRadius; ++x) {
      if (onImage(x, y) && !used[x][y] && probability[x][y] > 0) {
        used[x][y] = true;
        zone.push_back({{x, y}, probability[x][y]});
        findZone(x, y, probability, used, tileRadius, zone);
      }
    }
  }
}

void drawSquare(const int &row, const int &col, Matrix<qreal> &matrix) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  const auto onImage = [width, height](const int &x, const int &y) -> bool {
    return x >= 0 && y >= 0 && x < width && y < height;
  };

  const auto radius = 3;

  const auto left = col - radius;
  const auto right = col + radius;

  for (int x = row - radius; x <= row + radius; ++x) {
    if (onImage(x, left)) {
      matrix[x][left] = 255;
    }
    if (onImage(x, right)) {
      matrix[x][right] = 255;
    }
  }

  const auto up = row - radius;
  const auto down = row + radius;

  for (int y = col - radius; y <= col + radius; ++y) {
    if (onImage(up, y)) {
      matrix[up][y] = 255;
    }
    if (onImage(down, y)) {
      matrix[down][y] = 255;
    }
  }
}

Matrix<qreal> NMS_v2(const Matrix<qreal> &probability,
                     const Matrix<QPoint> &directions, const int &tileRadius,
                     QVector<Descriptor> &descriptors) {
  const auto width = probability.size();
  const auto height = probability[0].size();

  Matrix<qreal> result(width, Row<qreal>(height, 0));
  Matrix<bool> used(width, Row<bool>(height, false));

  QVector<QPair<QPoint, qreal>> zone;

  const auto diameter = 2 * tileRadius;
  const auto minZoneSize = diameter * diameter;

  int maxX, maxY;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (probability[x][y] > 0 && !used[x][y]) {
        zone.clear();

        findZone(x, y, probability, used, tileRadius, zone);

        // минимальный размер зоны - квадрат со стороной 2 * tileRadius
        // наверное ещё нужно смотреть на форму зоны, но пока так
        if (zone.size() >= minZoneSize) {
          auto max = *std::max_element(zone.begin(), zone.end(),
                                       [](const QPair<QPoint, qreal> &left,
                                          const QPair<QPoint, qreal> &right) {
                                         return left.second < right.second;
                                       });

          maxX = max.first.x();
          maxY = max.first.y();
          // просто чтобы нормально сохранить картинку
          result[maxX][maxY] = 255;

          drawSquare(max.first.x(), max.first.y(), result);

          descriptors.push_back({Direction::fromPoint(directions[maxX][maxY]),
                                 probability[x][y],
                                 {x, y}});
        }
      }
    }
  }

  return result;
}
