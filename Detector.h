#ifndef DETECTOR_H
#define DETECTOR_H

#include <QImage>
#include <QPoint>

#include "Descriptor.h"
#include "aliases.h"

class Detector {
public:
  Detector(const int &tileRadius, const qreal &radius,
           const qreal standardDeviation);

  QVector<Descriptor> detecting(const QImage &image);

private:
  int m_tileRadius;
  qreal m_radius;
  qreal m_standardDeviation;
};

#endif // DETECTOR_H
