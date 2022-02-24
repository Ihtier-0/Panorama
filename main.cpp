#include <QCoreApplication>

#include "ImageCombiner.h"
#include "testUtils/testUtils.h"

#include "FAST/FAST.h"
#include "blur/blur.h"
#include "utils/MatrixUtils.h"
#include "utils/YUVUtils.h"
#include "utils/utils.h"

int main(int argc, char *argv[]) {
  //  startTests(argc, argv);

  QCoreApplication a(argc, argv);

  QElapsedTimer timer;
  QImage image("house.jpg");

  timer.start();
  qDebug() << "brightness...";
  Matrix<qreal> brightness = YUV::getBrightness(image);
  toImage(brightness).save("1) brightness.jpg");
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  timer.start();
  qDebug() << "blur...";
  auto blur = blur::gaussian(brightness, 5, 1);
  toImage(blur).save("2) blur.jpg");
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  timer.start();
  qDebug() << "FAST...";
  auto fast = FAST(blur, 20);
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  const auto width = image.width();
  const auto height = image.height();

  auto overlay = Matrix<qreal>(width, Row<qreal>(height, 0));

  for (const auto &f : fast) {
    drawSquare(f.x(), f.y(), overlay);
  }

  drawOverlay(overlay, image);

  image.save("3) overlay.jpg");

  //  QImage right("right.jpg");
  //  QImage left("left.jpg");
  //  // делается чтобы видеть результат на монохромных картинках
  //  right.convertTo(QImage::Format_RGB888);
  //  left.convertTo(QImage::Format_RGB888);

  //  ImageCombiner combiner;
  //  combiner.combine(left, right);

  return 0;
}
