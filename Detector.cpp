#include "Detector.h"

#include <QImage>
#include <QVector2D>
#include <QVector>
#include <QtDebug>
#include <QtMath>

#include "Direction/Direction.h"
#include "Moravec/Moravec.h"
#include "blur/blur.h"
#include "utils/MatrixUtils.h"
#include "utils/YUVUtils.h"

Detector::Detector(const int &tileRadius, const qreal &radius,
                   const qreal standardDeviation)
    : m_tileRadius(tileRadius), m_radius(radius),
      m_standardDeviation(standardDeviation) {}

QImage Detector::detecting(const QImage &image) {
  Matrix<qreal> brightness = YUV::getBrightness(image);

  toImage(brightness).save("brightness.jpg");

  auto blur = blur::gaussian(brightness, m_radius, m_standardDeviation);

  toImage(blur).save("blur.jpg");

  const auto width = image.width();
  const auto height = image.height();

  const auto directions = Direction::all();
  const auto directionsCount = directions.size();

  QVector<qreal> probabilities(directionsCount);

  Matrix<qreal> probability(width, Row<qreal>(height, 0));
  Matrix<QPoint> direction(width, Row<QPoint>(height));

  qreal avg = 0;
  qreal pixelCount = width * height;
  qreal minProbability;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      for (int directionIndex = 0; directionIndex < directionsCount;
           ++directionIndex) {
        probabilities[directionIndex] = Moravec::probablity(
            blur, x, y, Direction::toPoint(directions[directionIndex]),
            m_tileRadius);
      }
      minProbability =
          *std::min_element(probabilities.begin(), probabilities.end());
      probability[x][y] = minProbability;
      direction[x][y] =
          Direction::toPoint(directions[probabilities.indexOf(minProbability)]);

      avg += minProbability / pixelCount;
    }
  }

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (probability[x][y] < avg) {
        probability[x][y] = 0;
      }
    }
  }

  const auto toColor = [](const QPoint &point) -> QColor {
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
  };

  QImage directionImage(width, height, QImage::Format_RGB888);
  directionImage.fill(QColorConstants::Yellow);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (probability[x][y] > 0) {
        directionImage.setPixelColor(x, y, toColor(direction[x][y]));
      }
    }
  }

  directionImage.save("directionImage.jpg");

  return (QImage());
}
