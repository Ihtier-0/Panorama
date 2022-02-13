#include "GradientDescent.h"

#include "../../utils/utils.h"

QVector<qreal>
Optimization::gradient(std::function<qreal(const QVector<qreal> &)> function,
                       const QVector<qreal> &point, const qreal &step) {
  const auto size = point.size();
  QVector<qreal> gradient(size);
  QVector<qreal> left, right;

  for (int i = 0; i < size; ++i) {
    left = point;
    right = point;

    left[i] += step;
    right[i] -= step;

    gradient[i] = (function(left) - function(right)) / (2 * step);
  }

  return gradient;
}

QVector<qreal> Optimization::GradientDescent(
    std::function<qreal(const QVector<qreal> &)> function,
    const QVector<qreal> &point, const qreal &step, const int &iteration) {
  QVector<qreal> result = point;
  QVector<qreal> gradient;

  for (int i = 0; i < iteration; ++i) {
    gradient = Optimization::gradient(function, result, step);
    result = sum(result, multiply(-0.5, gradient));
  }

  return result;
}
