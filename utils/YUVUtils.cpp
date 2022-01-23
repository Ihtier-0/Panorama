#include "YUVUtils.h"

namespace YUV {

qreal getBrightness(const QColor &color) {
  // numbers from:
  // https://en.wikipedia.org/wiki/Luma_(video)
  // https://ru.wikipedia.org/wiki/%D0%9E%D1%82%D1%82%D0%B5%D0%BD%D0%BA%D0%B8_%D1%81%D0%B5%D1%80%D0%BE%D0%B3%D0%BE
  return 0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue();
}

Matrix<qreal> getBrightness(const QImage &image) {
  const auto width = image.width();
  const auto height = image.height();

  Matrix<qreal> brightness(width, Row<qreal>(height, 0));
  QColor color;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      color = image.pixelColor(x, y);
      brightness[x][y] = getBrightness(color);
    }
  }

  return brightness;
}

} // namespace YUV
