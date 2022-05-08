#ifndef FAST_H
#define FAST_H

#include "../aliases.h"
#include <QVector2D>
#include <QVector>

// возможно когда-нибудь пригодится
enum class SupportedRadius { three };

int SupportedRadiusToInt(const SupportedRadius &radius);

QVector<QVector2D>
FAST(const Matrix<float> &brightness, const float &t,
     const SupportedRadius &supportedRadius = SupportedRadius::three);

#endif // FAST_H
