#ifndef MATRIXUTILS_H
#define MATRIXUTILS_H

#include <QImage>

#include "../aliases.h"

QImage toImage(const Matrix<qreal> &matrix);
void drawOverlay(const Matrix<qreal> &overlay, QImage &image);

#endif // MATRIXUTILS_H
