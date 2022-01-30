#ifndef MORAVEC_H
#define MORAVEC_H

#include <QPoint>

#include "../aliases.h"

namespace Moravec {

/** return probabilities avg
 */
qreal probablity(const Matrix<qreal> &blur, const int &tileRadius,
                 Matrix<qreal> &probability, Matrix<QPoint> &directionsMatrix);

qreal probablity(const Matrix<qreal> &matrix, const int &row, const int &col,
                 const QPoint &offset, const int &tileRadius);

void clipping(Matrix<qreal> &probability, Matrix<QPoint> &directionsMatrix,
              const qreal &clippingConstant);
} // namespace Moravec

#endif // MORAVEC_H
