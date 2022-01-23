#include "MatrixUtils.h"

QImage toImage(const Matrix<qreal> &matrix) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  QImage image(width, height, QImage::Format_RGB888);

  qreal color;

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      color = matrix[i][j];
      image.setPixelColor(i, j, QColor(color, color, color));
    }
  }

  return image;
}
