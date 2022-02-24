#ifndef FAST_H
#define FAST_H

#include "../aliases.h"
#include <QVector2D>
#include <QVector>

// мозможно когда-нибудь пригодится
enum class SupportedRadius { three };

int SupportedRadiusToInt(const SupportedRadius &radius);

QVector<QVector2D>
FAST(const Matrix<qreal> &brightness, const qreal &t,
     const SupportedRadius &supportedRadius = SupportedRadius::three);

#endif // FAST_H
