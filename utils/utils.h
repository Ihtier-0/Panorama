#ifndef UTILS_H
#define UTILS_H

#include <QColor>
#include <QTransform>

QColor randomColor();
QTransform operator+(const QTransform &left, const QTransform &right);

#endif // UTILS_H
