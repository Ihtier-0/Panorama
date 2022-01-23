#ifndef BLUR_H
#define BLUR_H

#include <QPoint>

#include "../aliases.h"

namespace blur {
qreal gaussianFunction(const qreal &x, const qreal &y,
                       const qreal &standardDeviation);
qreal gaussian(const Matrix<qreal> &matrix, const qreal &radius,
               const qreal &standardDeviation, const QPoint &center);
Matrix<qreal> gaussian(const Matrix<qreal> &matrix, const qreal &radius,
                       const qreal &standardDeviation);
} // namespace blur

#endif // BLUR_H
