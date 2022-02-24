#include "FAST.h"

QVector<QVector2D> FAST(const Matrix<qreal> &brightness, const qreal &t,
                        const SupportedRadius &supportedRadius) {
  const auto width = brightness.size();
  const auto height = brightness[0].size();

  int radius = SupportedRadiusToInt(supportedRadius);

  qreal currentBrightness;

  QVector<QVector2D> features;

  for (int y = 0 + radius; y < height - radius; ++y) {
    for (int x = 0 + radius; x < width - radius; ++x) {
      currentBrightness = brightness[x][y];
      if (currentBrightness > brightness[x + radius][y] + t &&
          currentBrightness > brightness[x - radius][y] + t &&
          currentBrightness > brightness[x][y + radius] + t &&
          currentBrightness > brightness[x][y - radius] + t) {
        if (currentBrightness > brightness[x + radius][y + 1] + t &&
            currentBrightness > brightness[x + radius][y - 1] + t &&
            currentBrightness > brightness[x - radius][y + 1] + t &&
            currentBrightness > brightness[x - radius][y - 1] + t &&
            currentBrightness > brightness[x + 1][y + radius] + t &&
            currentBrightness > brightness[x - 1][y + radius] + t &&
            currentBrightness > brightness[x + 1][y - radius] + t &&
            currentBrightness > brightness[x - 1][y - radius] + t) {
          features.push_back(QVector2D(x, y));
        }
      }
      if (currentBrightness < brightness[x + radius][y] - t &&
          currentBrightness < brightness[x - radius][y] - t &&
          currentBrightness < brightness[x][y + radius] - t &&
          currentBrightness < brightness[x][y - radius] - t) {
        if (currentBrightness < brightness[x + radius][y + 1] - t &&
            currentBrightness < brightness[x + radius][y - 1] - t &&
            currentBrightness < brightness[x - radius][y + 1] - t &&
            currentBrightness < brightness[x - radius][y - 1] - t &&
            currentBrightness < brightness[x + 1][y + radius] - t &&
            currentBrightness < brightness[x - 1][y + radius] - t &&
            currentBrightness < brightness[x + 1][y - radius] - t &&
            currentBrightness < brightness[x - 1][y - radius] - t) {
          features.push_back(QVector2D(x, y));
        }
      }
    }
  }

  return features;
}

int SupportedRadiusToInt(const SupportedRadius &radius) {
  switch (radius) {
  case SupportedRadius::three: {
    return 3;
  }
  }

  return 0;
}
