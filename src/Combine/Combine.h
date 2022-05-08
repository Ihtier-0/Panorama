#ifndef COMBINE_H
#define COMBINE_H

#include <QVector2D>

QVector2D initialApproximation(
    const QVector<QVector2D> &leftFast, const QVector<QVector2D> &rightFast,
    const QVector<QBitArray> &leftBRIEF, const QVector<QBitArray> &rightBRIEF,
    const QVector<QPair<int, int>> &bestSimilar,
    const std::function<QPoint(const QPoint &)> &rightToBothCoord);

#endif // COMBINE_H
