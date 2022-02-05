#include <QCoreApplication>

#include "ImageCombiner.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QImage right("right.jpg");
  QImage left("left.jpg");
  // делается чтобы видеть результат на монохромных картинках
  right.convertTo(QImage::Format_RGB888);
  left.convertTo(QImage::Format_RGB888);

  ImageCombiner combiner;
  combiner.combine(left, right);

  return 0;
}
