#include "Moravec.h"

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
