#include "Moravec.h"

#include "../Direction/Direction.h"

qreal Moravec::probablity(const Matrix<qreal> &matrix, const int &row,
                          const int &col, const QPoint &offset,
                          const int &tileRadius) {
  const int yOffset = offset.y();
  const int xOffset = offset.x();

  const auto width = matrix.size();
  const auto height = matrix[0].size();

  const auto onImage = [width, height](const int &x, const int &y) -> bool {
    return x >= 0 && y >= 0 && x < width && y < height;
  };

  qreal probablity = 0;
  qreal difference;

  for (int y = col - tileRadius; y <= col + tileRadius; ++y) {
    for (int x = row - tileRadius; x <= row + tileRadius; ++x) {
      if (onImage(x, y) && onImage(x + xOffset, y + yOffset)) {
        difference = matrix[x + xOffset][y + yOffset] - matrix[x][y];
        probablity += difference * difference;
      }
    }
  }

  return qFuzzyIsNull(probablity) ? Q_INFINITY : probablity;
}

qreal Moravec::probablity(const Matrix<qreal> &blur, const int &tileRadius,
                          Matrix<qreal> &probability,
                          Matrix<QPoint> &directionsMatrix) {
  const auto width = probability.size();
  const auto height = probability[0].size();

  const auto directions = Direction::all();
  const auto directionsCount = directions.size();

  QVector<qreal> probabilities(directionsCount);

  qreal avg = 0;
  qreal pixelCount = width * height;
  qreal minProbability;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      for (int directionIndex = 0; directionIndex < directionsCount;
           ++directionIndex) {
        probabilities[directionIndex] = Moravec::probablity(
            blur, x, y, Direction::toPoint(directions[directionIndex]),
            tileRadius);
      }
      minProbability =
          *std::min_element(probabilities.begin(), probabilities.end());
      probability[x][y] = minProbability;
      directionsMatrix[x][y] =
          Direction::toPoint(directions[probabilities.indexOf(minProbability)]);

      avg += minProbability / pixelCount;
    }
  }

  return avg;
}

void Moravec::clipping(Matrix<qreal> &probability,
                       Matrix<QPoint> &directionsMatrix,
                       const qreal &clippingConstant) {
  const auto width = probability.size();
  const auto height = probability[0].size();

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (probability[x][y] < clippingConstant) {
        directionsMatrix[x][y] = QPoint(0, 0);
        probability[x][y] = 0;
      }
    }
  }
}
