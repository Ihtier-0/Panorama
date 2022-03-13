#include "Combine.h"

#include <QBitArray>
#include <QDebug>
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
