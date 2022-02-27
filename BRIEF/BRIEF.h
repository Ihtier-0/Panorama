#ifndef BRIEF_H
#define BRIEF_H

#include "../aliases.h"
#include <QBitArray>
#include <QPoint>

QVector<QPair<QPoint, QPoint>> BRIEFSequence();

QBitArray BRIEF(const Matrix<qreal> &brightness, const int &x, const int &y,
                const QVector<QPair<QPoint, QPoint>> &BRIEFSequence);

QVector<QPair<int, int>> findSimilar(const QVector<QBitArray> &left,
                                     const QVector<QBitArray> &right);

#endif // BRIEF_H
