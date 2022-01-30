#include "Detector.h"

#include <QElapsedTimer>
#include <QImage>
#include <QVector2D>
#include <QVector>
#include <QtDebug>
#include <QtMath>

#include "Direction/Direction.h"
#include "Moravec/Moravec.h"
#include "NonMaximalSuppression/NonMaximalSuppression.h"
#include "blur/blur.h"
#include "utils/MatrixUtils.h"
#include "utils/YUVUtils.h"

#define SAVE
#define V2

Detector::Detector(const int &tileRadius, const qreal &radius,
                   const qreal standardDeviation)
    : m_tileRadius(tileRadius), m_radius(radius),
      m_standardDeviation(standardDeviation) {}

QVector<Descriptor> Detector::detecting(const QImage &image) {
  QElapsedTimer timer;

  timer.start();
  qDebug() << "brightness...";
  Matrix<qreal> brightness = YUV::getBrightness(image);
#ifdef SAVE
  toImage(brightness).save("1) brightness.jpg");
#endif
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  timer.start();
  qDebug() << "blur...";
  auto blur = blur::gaussian(brightness, m_radius, m_standardDeviation);
#ifdef SAVE
  toImage(blur).save("2) blur.jpg");
#endif
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  timer.start();
  qDebug() << "Moravec...";
  const auto width = image.width();
  const auto height = image.height();
  Matrix<qreal> probability(width, Row<qreal>(height, 0));
  Matrix<QPoint> directions(width, Row<QPoint>(height));
  qreal avg = Moravec::probablity(blur, m_tileRadius, probability, directions);
#ifdef SAVE
  Direction::toImage(directions).save("3) MoravecDirections.jpg");
#endif
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  timer.start();
  qDebug() << "clipping...";
  Moravec::clipping(probability, directions, avg);
#ifdef SAVE
  Direction::toImage(directions).save("4) DirectionsAfterClipping.jpg");
#endif
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  timer.start();
  qDebug() << "NMS...";
  QVector<Descriptor> descriptors;
#ifdef V2
  auto nms = NMS_v2(probability, directions, m_tileRadius, descriptors);
#else
  auto nms = NMS(probability, directions, m_tileRadius, descriptors);
#endif
  toImage(nms).save("5) nms.jpg");
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  timer.start();
  qDebug() << "result...";
  QImage result = image;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (nms[x][y] > 0) {
        result.setPixelColor(x, y, QColorConstants::Red);
      }
    }
  }

#ifdef SAVE
  result.save("6) result.jpg");
#endif
  qDebug() << "DONE!" << timer.elapsed() << "ms";
  return descriptors;
}
