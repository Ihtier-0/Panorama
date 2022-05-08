#include "blur.h"

#include "utils/utils.h"
#include <QtMath>

// -----------------------------------------------------------------------------

namespace blur {

// -----------------------------------------------------------------------------

float gaussianFunction(const float &aX, const float &aY,
                       const float &aStandardDeviation) {
  float divider = 2.0f * aStandardDeviation * aStandardDeviation;

  // clang-format off

  auto power =
      -(aX * aX + aY * aY)
 / // --------------------
             divider;

  return qExp(power)
    / // -----------
      (M_PI * divider);

  // clang-format on
}

// -----------------------------------------------------------------------------

Matrix<float> gaussian(const float &aStandardDeviation) {
  int size = qRound(6 * aStandardDeviation);
  if (!(size % 2)) {
    size += 1;
  }

  int halfSize = (size - 1) / 2;

  Matrix<float> gauss(size, Row<float>(size, 0));

  for (int x = -halfSize; x <= halfSize; ++x) {
    for (int y = -halfSize; y <= halfSize; ++y) {
      gauss[halfSize + x][halfSize + y] =
          gaussianFunction(x, y, aStandardDeviation);
    }
  }

  return gauss;
}

// -----------------------------------------------------------------------------

static float convolution(const Matrix<float> &aGaussian,
                         const Matrix<float> &aMatrix, const int &aMatrixX,
                         const int &aMatrixY) {
  int halfSize = (aGaussian.size() - 1) / 2;
  const auto width = aMatrix.size();
  const auto height = aMatrix[0].size();

  float sum = 0;

  const auto onMatrix = [&](const int &aX, const int &aY) -> bool {
    return aX >= 0 && aY >= 0 && aX < width && aY < height;
  };

  for (int x = -halfSize; x <= halfSize; ++x) {
    for (int y = -halfSize; y <= halfSize; ++y) {
      if (onMatrix(aMatrixX + x, aMatrixY + y)) {
        sum += aGaussian[halfSize + x][halfSize + y] *
               aMatrix[aMatrixX + x][aMatrixY + y];
      }
    }
  }

  return sum;
}

// -----------------------------------------------------------------------------

Matrix<float> gaussian(const Matrix<float> &aMatrix,
                       const qreal &aStandardDeviation) {
  const auto width = aMatrix.size();
  const auto height = aMatrix[0].size();

  const auto gaus = gaussian(aStandardDeviation);
  const auto sum = ::sum(gaus);

  Matrix<float> blurred(width, Row<float>(height, 0));

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      blurred[x][y] = convolution(gaus, aMatrix, x, y) / sum;
    }
  }

  return blurred;
}

// -----------------------------------------------------------------------------

} // namespace blur

// -----------------------------------------------------------------------------
