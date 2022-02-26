#ifndef IMAGECOMBINER_H
#define IMAGECOMBINER_H

#include "Descriptor/Descriptor.h"
#include <QImage>

class ImageCombiner {
public:
  static void combine(const QImage &left, const QImage &right);

  static int bestPair(const QVector<QPair<int, int>> &associations,
                      const QVector<Descriptor> &leftCorners,
                      const QVector<Descriptor> &rightCorners);
};

#endif // IMAGECOMBINER_H
