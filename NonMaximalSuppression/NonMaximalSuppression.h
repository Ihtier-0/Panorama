#ifndef NONMAXIMALSUPPRESSION_H
#define NONMAXIMALSUPPRESSION_H

#include <QImage>

#include "../Descriptor/Descriptor.h"
#include "../aliases.h"

/** return overlay
 */
Matrix<qreal> NMS(const Matrix<qreal> &probability,
                  const Matrix<QPoint> &directions, const int &tileRadius,
                  QVector<Descriptor> &descriptors);

/** return overlay
 */
Matrix<qreal> NMS_v2(const Matrix<qreal> &probability,
                     const Matrix<QPoint> &directions, const int &tileRadius,
                     QVector<Descriptor> &descriptors);

#endif // NONMAXIMALSUPPRESSION_H
