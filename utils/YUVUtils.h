#ifndef YUVUTILS_H
#define YUVUTILS_H

#include <QImage>

#include "../aliases.h"

namespace YUV {
qreal getBrightness(const QColor &color);
Matrix<qreal> getBrightness(const QImage &image);
} // namespace YUV

#endif // YUVUTILS_H
