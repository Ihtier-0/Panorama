#include <QCoreApplication>

#include "Detector.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QImage image("house.jpg");
  // делается чтобы видеть результат на монохромных картинках
  image.convertTo(QImage::Format_RGB888);

  Detector detector(3, 5, 1);

  detector.detecting(image);

  return 0;
}
