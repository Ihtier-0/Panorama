#include "blur.h"

#include <QtMath>

namespace blur {

qreal gaussianFunction(const qreal &x, const qreal &y,
                       const qreal &standardDeviation) {
  qreal divider = 2 * standardDeviation * standardDeviation;

  // clang-format off

  auto power =
      -(x * x + y * y)
 / // ----------------
           divider;

  return qExp(power)
    / // -----------
      (M_PI * divider);

  // clang-format on
}

qreal gaussian(const Matrix<qreal> &matrix, const qreal &radius,
               const qreal &standardDeviation, const QPoint &center) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  const auto onImage = [width, height](const int &x, const int &y) {
    return x >= 0 && x < width && y >= 0 && y < height;
  };

  qreal blur = 0;
  qreal GaussSum = 0;
  qreal GaussValue;

  const auto centerX = center.x();
  const auto centerY = center.y();

  for (int y = centerY - radius; y <= centerY + radius; ++y) {
    for (int x = centerX - radius; x <= centerX + radius; x++) {
      if (onImage(x, y)) {
        GaussValue =
            gaussianFunction(x - centerX, y - centerY, standardDeviation);
        GaussSum += GaussValue;
        blur += GaussValue * matrix[x][y];
      }
    }
  }

  return qFuzzyIsNull(GaussSum) ? 0 : blur / GaussSum;
}

Matrix<qreal> gaussian(const Matrix<qreal> &matrix, const qreal &radius,
                       const qreal &standardDeviation) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  Matrix<qreal> blurred(width, Row<qreal>(height, 0));

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      blurred[x][y] = gaussian(matrix, radius, standardDeviation, QPoint(x, y));
    }
  }

  return blurred;
}

} // namespace blur
