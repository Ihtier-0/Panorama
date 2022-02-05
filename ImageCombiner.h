#ifndef IMAGECOMBINER_H
#define IMAGECOMBINER_H

#include <QImage>

class ImageCombiner {
public:
  void combine(const QImage &left, const QImage &right);
};

#endif // IMAGECOMBINER_H
