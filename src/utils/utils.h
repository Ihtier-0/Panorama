#ifndef UTILS_H
#define UTILS_H

#include "../aliases.h"
#include <QColor>
#include <QTransform>
#include <QVector>

float sum(const Matrix<float> &matrix);

QColor randomColor();

QImage toImage(const Matrix<float> &matrix);

void drawSquare(QImage &aImage, const int &aRow, const int &aCol,
                const QColor &aColor);

namespace YUV {

/**
 * @brief //
 * https://ru.wikipedia.org/wiki/YUV#%D0%9A%D0%BE%D1%8D%D1%84%D1%84%D0%B8%D1%86%D0%B8%D0%B5%D0%BD%D1%82%D1%8B_KR_KB
 */
enum class Recommendation : char {
  BT601,
  BT709,
  BT2020,
};

float getBrightness(const QColor &aColor,
                    const Recommendation &aRecommendation);

Matrix<float> getBrightness(const QImage &aImage,
                            const Recommendation &aRecommendation);
} // namespace YUV

#endif // UTILS_H
