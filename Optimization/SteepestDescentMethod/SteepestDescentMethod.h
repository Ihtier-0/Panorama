#ifndef STEEPESTDESCENTMETHOD_H
#define STEEPESTDESCENTMETHOD_H

#include <QVector>

namespace Optimization {
QVector<qreal>
SteepestDescentMethod(std::function<qreal(const QVector<qreal> &)> function,
                      const QVector<qreal> &point, const qreal &step = 0.001,
                      const int &iteration = 1000);
} // namespace Optimization

#endif // STEEPESTDESCENTMETHOD_H
