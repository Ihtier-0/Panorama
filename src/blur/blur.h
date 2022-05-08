#ifndef BLUR_H
#define BLUR_H

#include <QPoint>

#include "aliases.h"

namespace blur {
float gaussian(const float &aX, const float &aY,
               const float &aStandardDeviation);

Matrix<float> gaussian(const float &aStandardDeviation);

Matrix<float> gaussian(const Matrix<float> &aMatrix,
                       const qreal &aStandardDeviation);
} // namespace blur

#endif // BLUR_H
