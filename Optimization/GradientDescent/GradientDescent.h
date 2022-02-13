#ifndef GRADIENTDESCENT_H
#define GRADIENTDESCENT_H

#include <QVector>

namespace Optimization {
QVector<qreal> gradient(std::function<qreal(const QVector<qreal> &)> function,
                        const QVector<qreal> &point, const qreal &step = 0.001);
QVector<qreal>
GradientDescent(std::function<qreal(const QVector<qreal> &)> function,
                const QVector<qreal> &point, const qreal &step = 0.001,
                const int &iteration = 1000);
} // namespace Optimization

#endif // GRADIENTDESCENT_H
