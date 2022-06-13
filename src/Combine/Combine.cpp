#include "Combine.h"

#include <QBitArray>
#include <QDebug>
#include <QTransform>
#include <QVector>

QVector2D initialApproximation(
    const QVector<QVector2D> &leftFast, const QVector<QVector2D> &rightFast,
    const QVector<QBitArray> &leftBRIEF, const QVector<QBitArray> &rightBRIEF,
    const QVector<QPair<int, int>> &bestSimilar,
    const std::function<QPoint(const QPoint &)> &rightToBothCoord) {
  const auto size = bestSimilar.size();

  int minIndex = 0;
  int minDiff =
      (leftBRIEF[bestSimilar[0].first] ^ rightBRIEF[bestSimilar[0].second])
          .count(false);
  int curDiff;

  for (int i = 0; i < size; ++i) {
    curDiff =
        (leftBRIEF[bestSimilar[i].first] ^ rightBRIEF[bestSimilar[i].second])
            .count(false);
    if (curDiff < minDiff) {
      minDiff = curDiff;
      minIndex = i;
    }
  }

  const auto left = leftFast[bestSimilar[minIndex].first];
  const auto right = rightFast[bestSimilar[minIndex].second];

  const auto leftBoth = left;
  const auto rightBoth = rightToBothCoord(right.toPoint());

  return leftBoth - QVector2D(rightBoth);
}

// -----------------------------------------------------------------------------

QVector<qreal> product(const qreal &number, const QVector<qreal> &vector) {
  quint64 size = vector.size();
  QVector<qreal> result(size);

  for (quint64 i = 0; i < size; ++i) {
    result[i] = vector[i] * number;
  }

  return result;
}

QVector<qreal> product(const QVector<qreal> &vector, const qreal &number) {
  return product(number, vector);
}

QVector<qreal> sum(const QVector<qreal> &left, const QVector<qreal> &right) {
  quint64 size = left.size();

  QVector<qreal> result(size);

  for (quint64 i = 0; i < size; ++i) {
    result[i] = left[i] + right[i];
  }

  return result;
}

QVector<qreal> grad(std::function<qreal(const QVector<qreal> &)> &f,
                    const QVector<qreal> &x) {
  qreal epsilon = 0.001;

  quint64 size = x.size();

  QVector<qreal> result(size), tmpLeft, tmpRight;

  for (quint64 i = 0; i < size; ++i) {
    tmpLeft = x;
    tmpRight = x;

    tmpLeft[i] += epsilon;
    tmpRight[i] -= epsilon;

    result[i] = (f(tmpLeft) - f(tmpRight)) / (2 * epsilon);
  }

  return result;
};

qreal norm(const QVector<qreal> &vector) {
  qreal sum = 0;

  quint64 size = vector.size();

  for (quint64 i = 0; i < size; ++i) {
    sum += vector[i] * vector[i];
  }

  return qSqrt(sum);
};

qreal derivative(const std::function<qreal(const qreal &)> &f,
                 const qreal &point) {
  qreal epsilon = 0.001;

  return (f(point + epsilon) - f(point - epsilon)) / (2 * epsilon);
}

qreal gradientDescent(const std::function<qreal(const qreal &)> &f,
                      const qreal &initialApproximation, const qreal &alpha,
                      const qreal &epsilon, const quint64 &numberOfIteration) {
  qreal u0 = initialApproximation;
  qreal a = alpha;
  qreal g;

  for (quint64 i = 0; i < numberOfIteration; ++i) {
    g = derivative(f, u0);

    if (qAbs(g) < epsilon) {
      break;
    }

    u0 = u0 - a * g;
  }

  return u0;
}

// Метод наискорейшего спуска
void SteepestDescentMethod(std::function<qreal(const QVector<qreal> &)> &f,
                           const QVector<qreal> &initialApproximation,
                           QVector<qreal> &result, const qreal &epsilon,
                           const quint64 &numberOfIteration) {
  QVector<qreal> u0 = initialApproximation;
  QVector<qreal> g;
  qreal a;

  std::function<qreal(const qreal &)> alphaFunction;

  for (quint64 i = 0; i < numberOfIteration; ++i) {
    g = grad(f, u0);

    if (norm(g) < epsilon) {
      break;
    }

    alphaFunction = [&](const qreal &alpha) -> qreal {
      return f(sum(u0, product(-alpha, g)));
    };

    a = gradientDescent(alphaFunction, 1, 0.1, epsilon, numberOfIteration);

    u0 = sum(u0, product(-a, g));
  }

  result = u0;
}

OptimizeResult optimize(
    const QVector<QVector2D> &leftFast, const QVector<QVector2D> &rightFast,
    const QVector<QBitArray> &leftBRIEF, const QVector<QBitArray> &rightBRIEF,
    const QVector<QPair<int, int>> &bestSimilar,
    const std::function<QPoint(const QPoint &)> &rightToBothCoord,
    const OptimizeResult &initialApproximation, const Loss &aLoss,
    const qreal &epsilon, const quint64 &numberOfIteration) {
  qreal minF = -1;
  QVector<qreal> minX = {
      initialApproximation.rotate,        initialApproximation.translate.x(),
      initialApproximation.translate.y(), initialApproximation.scale.x(),
      initialApproximation.scale.y(),
  };

  std::function<qreal(const QVector<qreal> &)> f;

  float BRIEFsum = 0;

  for (const auto &b : bestSimilar) {
    BRIEFsum += (leftBRIEF[b.first] ^ rightBRIEF[b.second]).count(true);
  }

  switch (aLoss) {
  case Loss::L1: {
    f = [&](const QVector<qreal> &x) -> qreal {
      qreal result = 0;

      QTransform transform;
      transform.scale(x[3], x[4]);
      transform.rotate(x[0]);
      transform.translate(x[1], x[2]);

      for (const auto &b : bestSimilar) {
        QPointF rightBoth = rightToBothCoord(rightFast[b.second].toPoint());
        result +=
            (leftFast[b.first] - QVector2D(transform.map(rightBoth))).length();
      }

      if (minF == -1) {
        minF = result;
      }

      if (minF != -1 && result < minF) {
        minF = result;
        minX = x;
      }

      return result + qSqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2] +
                            x[3] * x[3] + x[4] * x[4]);
    };
    break;
  }
  case Loss::L2: {
    f = [&](const QVector<qreal> &x) -> qreal {
      qreal result = 0;

      QTransform transform;
      transform.scale(x[3], x[4]);
      transform.rotate(x[0]);
      transform.translate(x[1], x[2]);

      for (const auto &b : bestSimilar) {
        QPointF rightBoth = rightToBothCoord(rightFast[b.second].toPoint());
        result +=
            ((leftBRIEF[b.first] ^ rightBRIEF[b.second]).count(true) /
             BRIEFsum) *
            (leftFast[b.first] - QVector2D(transform.map(rightBoth))).length();
      }

      if (minF == -1) {
        minF = result;
      }

      if (minF != -1 && result < minF) {
        minF = result;
        minX = x;
      }

      return result + qSqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2] +
                            x[3] * x[3] + x[4] * x[4]);
    };
    break;
  }
  }

  QVector<qreal> result;

  SteepestDescentMethod(f,
                        {
                            initialApproximation.rotate,
                            initialApproximation.translate.x(),
                            initialApproximation.translate.y(),
                            initialApproximation.scale.x(),
                            initialApproximation.scale.y(),
                        },
                        result, epsilon, numberOfIteration);

  return {minX[0],
          {static_cast<float>(minX[1]), static_cast<float>(minX[2])},
          {static_cast<float>(minX[3]), static_cast<float>(minX[4])}};
}
