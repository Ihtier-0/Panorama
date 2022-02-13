#include "GradientDescentTests.h"

#include "../../utils/utils.h"
#include "GradientDescent.h"

void GradientDescentTests::GradientTest1() {
  // arrange
  auto function = [](const QVector<qreal> &point) -> qreal {
    return sin(point[0]);
  };
  QVector<qreal> point = {0};
  qreal step = 0.001;

  // act
  auto result = Optimization::gradient(function, point, step);

  // assert
  QVERIFY(qAbs(result[0] - cos(0)) < 0.01);
}

void GradientDescentTests::GradientDescentTest1() {
  // arrange
  auto function = [](const QVector<qreal> &point) -> qreal {
    const auto x = point[0];
    const auto y = point[1];
    return x * x + y * y + 7;
  };
  QVector<qreal> point = {10, 10};
  qreal step = 0.001;

  // act
  auto result = Optimization::GradientDescent(function, point, step);

  // assert
  QVERIFY(qAbs(function(result) - 7) < 0.01);
}
