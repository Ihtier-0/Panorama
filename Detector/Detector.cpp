#include "Detector.h"

#include <QElapsedTimer>
#include <QImage>
#include <QRandomGenerator>
#include <QVector2D>
#include <QVector>
#include <QtDebug>
#include <QtMath>

#include "../Direction/Direction.h"
#include "../Moravec/Moravec.h"
#include "../NonMaximalSuppression/NonMaximalSuppression.h"
#include "../blur/blur.h"
#include "../utils/MatrixUtils.h"
#include "../utils/YUVUtils.h"

#define SAVE
#define V2

// число подобранно эксперементально
// надо будет добавить возможность изменять её через gui
constexpr int reducedConstant = 1000;

Detector::Detector(const int &tileRadius, const qreal &radius,
                   const qreal standardDeviation, const clippingMode &mode)
    : m_tileRadius(tileRadius), m_radius(radius),
      m_standardDeviation(standardDeviation), m_mode(mode) {}

QVector<Descriptor> Detector::detecting(const QImage &image, QImage *result) {
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
  qreal clippingConstant =
      Moravec::probablity(blur, m_tileRadius, probability, directions);
  switch (m_mode) {
  case clippingMode::avg: {
    break;
  }
  case clippingMode::halfAvg: {
    clippingConstant /= 2;
    break;
  }
  case clippingMode::edgeMaxApproximation: {
    clippingConstant = edgeMaxApproximation();
    break;
  }
  case clippingMode::reducedEdgeMaxApproximation: {
    clippingConstant = edgeMaxApproximation() / reducedConstant;
    break;
  }
  case clippingMode::randomProbability: {
    auto random = QRandomGenerator::global();
    clippingConstant =
        probability[random->bounded(width)][random->bounded(height)];
    break;
  }
  }
#ifdef SAVE
  Direction::toImage(directions).save("3) MoravecDirections.jpg");
#endif
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  timer.start();
  qDebug() << "clipping...";
  Moravec::clipping(probability, directions, clippingConstant);
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

#ifdef SAVE
  toImage(nms).save("5) nms.jpg");
#endif
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  timer.start();
  qDebug() << "drawOverlay...";
  QImage resultSave = image;

  drawOverlay(nms, resultSave);
  if (result) {
    *result = resultSave;
  }
#ifdef SAVE
  resultSave.save("6) originalWithOverlay.jpg");
#endif
  qDebug() << "DONE!" << timer.elapsed() << "ms";
  return descriptors;
}

// максмальное возможное значение вероятности
// если считать её только по границе в диагональном направлении
qreal Detector::edgeMaxApproximation() {
  const auto maxBrightness = YUV::getBrightness(QColor::fromRgb(255, 255, 255));
  const auto squareMaxBrightness = maxBrightness * maxBrightness;

  // диаметр считается по такой формуле потому что центр не входит в радиус
  const auto diameter = 1 + 2 * m_tileRadius;

  return (2 * diameter - 1) * squareMaxBrightness;
}
