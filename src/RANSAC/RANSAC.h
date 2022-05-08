#ifndef RANSAC_H
#define RANSAC_H

#include <QBitArray>
#include <QPair>

QVector<QPair<int, int>> RANSAC(const QVector<QPair<int, int>> &similar,
                                const QVector<QBitArray> &leftBRIEF,
                                const QVector<QBitArray> &rightBRIEF,
                                const quint32 &resultCount,
                                const quint32 &iteration);

#endif // RANSAC_H
