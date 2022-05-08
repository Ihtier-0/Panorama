#include "utils.h"

#include <QImage>
#include <QRandomGenerator>
#include <QtMath>

// -----------------------------------------------------------------------------

float sum(const Matrix<float> &matrix) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  float sum = 0.0f;

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      sum += matrix[x][y];
    }
  }

  return sum;
}

// -----------------------------------------------------------------------------

QColor randomColor() {
  const auto random = QRandomGenerator::global();
  return QColor(random->bounded(256), random->bounded(256),
                random->bounded(256));
}

// -----------------------------------------------------------------------------

QImage toImage(const Matrix<float> &matrix) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  QImage image(width, height, QImage::Format_RGB888);

  float color;

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      color = matrix[i][j];
      image.setPixelColor(i, j, QColor(color, color, color));
    }
  }

  return image;
}

// -----------------------------------------------------------------------------

void drawSquare(QImage &aImage, const int &aRow, const int &aCol,
                const QColor &aColor) {
  const auto width = aImage.width();
  const auto height = aImage.height();

  const auto onImage = [width, height](const int &x, const int &y) -> bool {
    return x >= 0 && y >= 0 && x < width && y < height;
  };

  const auto radius = 3;

  const auto left = aCol - radius;
  const auto right = aCol + radius;

  for (int x = aRow - radius; x <= aRow + radius; ++x) {
    if (onImage(x, left)) {
      aImage.setPixelColor(x, left, aColor);
    }
    if (onImage(x, right)) {
      aImage.setPixelColor(x, right, aColor);
    }
  }

  const auto up = aRow - radius;
  const auto down = aRow + radius;

  for (int y = aCol - radius; y <= aCol + radius; ++y) {
    if (onImage(up, y)) {
      aImage.setPixelColor(up, y, aColor);
    }
    if (onImage(down, y)) {
      aImage.setPixelColor(down, y, aColor);
    }
  }
}

// -----------------------------------------------------------------------------

namespace YUV {

// -----------------------------------------------------------------------------

float getBrightness(const QColor &aColor,
                    const Recommendation &aRecommendation) {
  float KR, KB, KG;
  switch (aRecommendation) {
  case Recommendation::BT601: {
    KR = 0.299;
    KB = 0.114;
    break;
  }
  case Recommendation::BT709: {
    KR = 0.2126;
    KB = 0.0722;
    break;
  }
  case Recommendation::BT2020: {
    KR = 0.2627;
    KB = 0.0593;
    break;
  }
  }
  KG = 1 - KR - KB;

  return KR * aColor.red() + KG * aColor.green() + KB * aColor.blue();
}

// -----------------------------------------------------------------------------

Matrix<float> getBrightness(const QImage &aImage,
                            const Recommendation &aRecommendation) {
  const auto width = aImage.width();
  const auto height = aImage.height();

  Matrix<float> brightness(width, Row<float>(height, 0));
  QColor color;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      color = aImage.pixelColor(x, y);
      brightness[x][y] = getBrightness(color, aRecommendation);
    }
  }

  return brightness;
}

// -----------------------------------------------------------------------------

} // namespace YUV
