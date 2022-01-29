#ifndef NONMAXIMALSUPPRESSION_H
#define NONMAXIMALSUPPRESSION_H

#include <QImage>

#include "../aliases.h"

Matrix<qreal> NMS(const Matrix<qreal> &probability,
                  const Matrix<QPoint> &directions);

#endif // NONMAXIMALSUPPRESSION_H
