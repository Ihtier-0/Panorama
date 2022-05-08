#ifndef BRIEF_H
#define BRIEF_H

#include "../aliases.h"
#include <QBitArray>
#include <QPoint>

QVector<QPair<QPoint, QPoint>> BRIEFSequence();

QBitArray BRIEF(const Matrix<float> &aBrightness, const int &aX, const int &aY,
                const QVector<QPair<QPoint, QPoint>> &BRIEFSequence);

QVector<QPair<int, int>> findSimilar(const QVector<QBitArray> &aLeft,
                                     const QVector<QBitArray> &aRight);

#endif // BRIEF_H
