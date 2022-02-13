#ifndef UTILS_H
#define UTILS_H

#include <QColor>
#include <QTransform>
#include <QVector>

QColor randomColor();
QVector<qreal> multiply(const qreal &number, const QVector<qreal> &vector);
QVector<qreal> multiply(const QVector<qreal> &vector, const qreal &number);
QVector<qreal> sum(const QVector<qreal> &left, const QVector<qreal> &right);
qreal norm(const QVector<qreal> &vector);

#endif // UTILS_H
