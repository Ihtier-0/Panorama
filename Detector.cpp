#include "Detector.h"

#include <QImage>
#include <QVector2D>
#include <QVector>
#include <QtDebug>
#include <QtMath>

Detector::Detector(const QSize &tileSize, const qreal &radius,
                   const qreal standardDeviation)
    : m_tileSize(tileSize), m_radius(radius),
      m_standardDeviation(standardDeviation) {}

QImage Detector::detecting(const QImage &image) {
  const auto width = image.width();
  const auto height = image.height();

  Matrix<qreal> brightness = getYUVBrightness(image);

  matrixToImage(brightness).save("brightness.jpg");

  auto blur = gaussianBlur(brightness, m_radius, m_standardDeviation);

  matrixToImage(blur).save("blur.jpg");

  // clang-format off
  const QVector<QVector2D> offsets{{ 1,  0},
                                   { 1,  1},
                                   { 0,  1},
                                   {-1,  1},
                                   {-1,  0},
                                   {-1, -1},
                                   { 0, -1},
                                   { 1, -1}};
  // clang-format on

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      for (const auto &offset : offsets) {
      }
    }
  }

  return (QImage());
}

qreal Detector::getYUVBrightness(const QColor &color) {
  // numbers from:
  // https://en.wikipedia.org/wiki/Luma_(video)
  // https://ru.wikipedia.org/wiki/%D0%9E%D1%82%D1%82%D0%B5%D0%BD%D0%BA%D0%B8_%D1%81%D0%B5%D1%80%D0%BE%D0%B3%D0%BE
  return 0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue();
}

Matrix<qreal> Detector::getYUVBrightness(const QImage &image) {
  const auto width = image.width();
  const auto height = image.height();

  Matrix<qreal> brightness(width, Row<qreal>(height, 0));

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      brightness[i][j] = getYUVBrightness(image.pixelColor(i, j));
    }
  }

  return brightness;
}

QImage Detector::matrixToImage(const Matrix<qreal> &matrix) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  QImage image(width, height, QImage::Format_RGB888);

  qreal brightness;

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      brightness = matrix[i][j];
      image.setPixelColor(i, j, QColor(brightness, brightness, brightness));
    }
  }

  return image;
}

static qreal GaussianFunction(const qreal &x, const qreal &y,
                              const qreal &standardDeviation) {
  qreal divider = 2 * standardDeviation * standardDeviation;

  return qExp(-(x * x + y * y) / divider) / (2 * M_PI * divider);
}

qreal Detector::gaussianBlur(const Matrix<qreal> &matrix, const qreal &radius,
                             const qreal &standardDeviation, const int &x,
                             const int &y) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  const auto inImage = [width, height](const int &x, const int &y) {
    return x >= 0 && x < width && y >= 0 && y < height;
  };

  qreal blur = 0;
  qreal GaussSum = 0;
  qreal GaussValue;

  for (int i = x - radius; i <= x + radius; i++) {
    for (int j = y - radius; j <= y + radius; ++j) {
      if (inImage(i, j)) {
        GaussValue = GaussianFunction(i - x, j - y, standardDeviation);
        GaussSum += GaussValue;
        blur += GaussValue * matrix[i][j];
      }
    }
  }

  return qFuzzyIsNull(GaussSum) ? 0 : blur / GaussSum;
}

Matrix<qreal> Detector::gaussianBlur(const Matrix<qreal> &matrix,
                                     const qreal &radius,
                                     const qreal &standardDeviation) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  Matrix<qreal> blurred(width, Row<qreal>(height, 0));

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      blurred[x][y] = gaussianBlur(matrix, radius, standardDeviation, x, y);
    }
  }

  return blurred;
}
