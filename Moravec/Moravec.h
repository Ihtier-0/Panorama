#ifndef MORAVEC_H
#define MORAVEC_H

#include <QPoint>

#include "../aliases.h"

namespace Moravec {
qreal probablity(const Matrix<qreal> &matrix, const int &row, const int &col,
                 const QPoint &offset, const int &tileRadius);
}

#endif // MORAVEC_H
