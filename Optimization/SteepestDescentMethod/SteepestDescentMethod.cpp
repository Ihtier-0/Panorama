#include "SteepestDescentMethod.h"

#include "../../utils/utils.h"
#include "../GradientDescent/GradientDescent.h"

QVector<qreal> Optimization::SteepestDescentMethod(
    std::function<qreal(const QVector<qreal> &)> function,
    const QVector<qreal> &point, const qreal &step, const int &iteration) {
  QVector<qreal> result = point;
  QVector<qreal> gradient;

  for (int i = 0; i < iteration; ++i) {
    gradient = Optimization::gradient(function, result, step);

    const auto f = [&](const QVector<qreal> &vector) {
      return function(sum(result, multiply(vector[0], gradient)));
    };

    const auto alpha = Optimization::GradientDescent(f, {-1}, step, iteration);

    result = sum(result, multiply(alpha[0], gradient));
  }

  return result;
}
