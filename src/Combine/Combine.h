#ifndef COMBINE_H
#define COMBINE_H

#include <QVector2D>

enum class Loss {
  L1,
  L2,
};

QVector2D initialApproximation(
    const QVector<QVector2D> &leftFast, const QVector<QVector2D> &rightFast,
    const QVector<QBitArray> &leftBRIEF, const QVector<QBitArray> &rightBRIEF,
    const QVector<QPair<int, int>> &bestSimilar,
    const std::function<QPoint(const QPoint &)> &rightToBothCoord);

struct OptimizeResult {
  qreal rotate;
  QVector2D translate;
  QVector2D scale;
};

OptimizeResult optimize(
    const QVector<QVector2D> &leftFast, const QVector<QVector2D> &rightFast,
    const QVector<QBitArray> &leftBRIEF, const QVector<QBitArray> &rightBRIEF,
    const QVector<QPair<int, int>> &bestSimilar,
    const std::function<QPoint(const QPoint &)> &rightToBothCoord,
    const OptimizeResult &initialApproximation, const Loss &aLoss,
    const qreal &epsilon = 0.001, const quint64 &numberOfIteration = 1000);

#endif // COMBINE_H
