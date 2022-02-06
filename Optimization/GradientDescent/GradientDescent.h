#ifndef GRADIENTDESCENT_H
#define GRADIENTDESCENT_H

#include <QTransform>

namespace Optimization {
QTransform gradient(std::function<qreal(const QTransform &)> function,
                    const QTransform &point, const qreal &step = 0.001);
QTransform GradientDescent(std::function<qreal(const QTransform &)> function,
                           const QTransform &point, const qreal &step = 0.001,
                           const int &iteration = 1000);
} // namespace Optimization

#endif // GRADIENTDESCENT_H
