#include <QCoreApplication>

#include "Detector.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QImage image("test.jpg");

  Detector detector(3, 5, 1);

  detector.detecting(image);

  return 0;
}
