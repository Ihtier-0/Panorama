#ifndef DETECTOR_H
#define DETECTOR_H

#include <QImage>
#include <QPoint>

#include "../Descriptor/Descriptor.h"
#include "aliases.h"

class Detector {
public:
  enum class clippingMode {
    avg,
    halfAvg,
    edgeMaxApproximation,
    reducedEdgeMaxApproximation,
    randomProbability
  };

  Detector(const int &tileRadius, const qreal &radius,
           const qreal standardDeviation, const clippingMode &mode);

  QVector<Descriptor> detecting(const QImage &image, QImage *result = nullptr);

private:
  qreal edgeMaxApproximation();

  int m_tileRadius;
  qreal m_radius;
  qreal m_standardDeviation;
  clippingMode m_mode;
};

#endif // DETECTOR_H
