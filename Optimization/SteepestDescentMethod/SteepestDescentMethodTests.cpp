#include "SteepestDescentMethodTests.h"

#include "SteepestDescentMethod.h"

void SteepestDescentMethodTests::SteepestDescentMethodTest1() {
  // arrange
  auto function = [](const QVector<qreal> &point) -> qreal {
    const auto x = point[0];
    const auto y = point[1];
    return x * x + y * y + 7;
  };
  QVector<qreal> point = {10, 10};
  qreal step = 0.001;

  // act
  auto result = Optimization::SteepestDescentMethod(function, point, step);

  qDebug() << result;

  // assert
  QVERIFY(qAbs(function(result) - 7) < 0.01);
}
