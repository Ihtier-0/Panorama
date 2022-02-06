#include "GradientDescent.h"

#include "../../utils/utils.h"

QTransform
Optimization::gradient(std::function<qreal(const QTransform &)> function,
                       const QTransform &point, const qreal &step) {
  const QVector<qreal> vectorPoint = {point.m11(), point.m12(), point.m13(),
                                      point.m21(), point.m22(), point.m23(),
                                      point.m31(), point.m32(), point.m33()};
  const auto size = vectorPoint.size();
  QVector<qreal> gradient(size, 0);
  QVector<qreal> leftVectorPoint, rightVectorPoint;
  QTransform left, right;
  qreal leftLoss;
  qreal rightLoss;

  for (qint64 i = 0; i < size; ++i) {
    if (i == 2 || i == 5 || i == 6 || i == 7 || i == 8) {
      continue;
    }
    leftVectorPoint = vectorPoint;
    leftVectorPoint[i] += step;

    rightVectorPoint = vectorPoint;
    rightVectorPoint[i] -= step;

    // clang-format off
   left.setMatrix(leftVectorPoint[0], leftVectorPoint[1], leftVectorPoint[2],
                  leftVectorPoint[3], leftVectorPoint[4], leftVectorPoint[5],
                  leftVectorPoint[6], leftVectorPoint[7], leftVectorPoint[8]);

   right.setMatrix(rightVectorPoint[0], rightVectorPoint[1], rightVectorPoint[2],
                   rightVectorPoint[3], rightVectorPoint[4], rightVectorPoint[5],
                   rightVectorPoint[6], rightVectorPoint[7], rightVectorPoint[8]);
    // clang-format on

    leftLoss = function(left);
    rightLoss = function(right);

    gradient[i] = (leftLoss - rightLoss) / (2 * step);
  }

  // clang-format off
 return QTransform(gradient[0], gradient[1], gradient[2],
                   gradient[3], gradient[4], gradient[5],
                   gradient[6], gradient[7], gradient[8]);
  // clang-format on
}

QTransform
Optimization::GradientDescent(std::function<qreal(const QTransform &)> function,
                              const QTransform &point, const qreal &step,
                              const int &iteration) {
  QTransform result = point;
  QTransform gradient;

  for (int i = 0; i < iteration; ++i) {
    gradient = Optimization::gradient(function, point, step);
    gradient *= -1;
    result = result + gradient;
  }

  return result;
}
