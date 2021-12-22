#include <QCoreApplication>

#include "Detector.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QImage image("habr.jpg");

  Detector detector(QSize(3, 3), 3, 1);

  detector.detecting(image);

  //  return a.exec();
  return 0;
}
