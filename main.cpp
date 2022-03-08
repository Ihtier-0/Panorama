#include <QCoreApplication>

#include <QPainter>

#include "ImageCombiner.h"
#include "testUtils/testUtils.h"

#include "BRIEF/BRIEF.h"
#include "FAST/FAST.h"
#include "RANSAC/RANSAC.h"
#include "blur/blur.h"
#include "utils/MatrixUtils.h"
#include "utils/YUVUtils.h"
#include "utils/utils.h"

int main(int argc, char *argv[]) {
  //  startTests(argc, argv);

  QCoreApplication a(argc, argv);

  QImage left("rio-00.png");
  QImage right("rio-01.png");
  QVector<QVector2D> leftFast, rightFast;
  Matrix<qreal> leftBlur, rightBlur;
  qreal fastT = 20;

  {
    QElapsedTimer timer;
    left.convertTo(QImage::Format_RGB888);

    timer.start();
    qDebug() << "brightness...";
    Matrix<qreal> brightness = YUV::getBrightness(left);
    toImage(brightness).save("1) left brightness.jpg");
    qDebug() << "DONE!" << timer.elapsed() << "ms";

    timer.start();
    qDebug() << "blur...";
    leftBlur = blur::gaussian(brightness, 5, 1);
    toImage(leftBlur).save("2) left blur.jpg");
    qDebug() << "DONE!" << timer.elapsed() << "ms";

    timer.start();
    qDebug() << "FAST...";
    leftFast = FAST(leftBlur, fastT);
    qDebug() << "DONE!" << timer.elapsed() << "ms";

    const auto width = left.width();
    const auto height = left.height();

    auto overlay = Matrix<qreal>(width, Row<qreal>(height, 0));

    for (const auto &f : leftFast) {
      drawSquare(f.x(), f.y(), overlay);
    }

    drawOverlay(overlay, left);

    left.save("3) left overlay.jpg");
  }

  {
    QElapsedTimer timer;
    right.convertTo(QImage::Format_RGB888);

    timer.start();
    qDebug() << "brightness...";
    Matrix<qreal> brightness = YUV::getBrightness(right);
    toImage(brightness).save("1) right brightness.jpg");
    qDebug() << "DONE!" << timer.elapsed() << "ms";

    timer.start();
    qDebug() << "blur...";
    rightBlur = blur::gaussian(brightness, 5, 1);
    toImage(rightBlur).save("2) right blur.jpg");
    qDebug() << "DONE!" << timer.elapsed() << "ms";

    timer.start();
    qDebug() << "FAST...";
    rightFast = FAST(rightBlur, fastT);
    qDebug() << "DONE!" << timer.elapsed() << "ms";

    const auto width = right.width();
    const auto height = right.height();

    auto overlay = Matrix<qreal>(width, Row<qreal>(height, 0));

    for (const auto &f : rightFast) {
      drawSquare(f.x(), f.y(), overlay);
    }

    drawOverlay(overlay, right);

    right.save("3) right overlay.jpg");
  }

  const auto sequence = BRIEFSequence();
  QVector<QBitArray> leftBRIEF, rightBRIEF;

  QElapsedTimer timer;
  timer.start();
  qDebug() << "BRIEF...";
  for (const auto left : leftFast) {
    leftBRIEF.push_back(BRIEF(leftBlur, left.x(), left.y(), sequence));
  }

  for (const auto right : rightFast) {
    rightBRIEF.push_back(BRIEF(rightBlur, right.x(), right.y(), sequence));
  }
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  const auto similar = findSimilar(leftBRIEF, rightBRIEF);

  const auto leftWidht = left.width();
  const auto rightWidht = right.width();

  const auto leftHeight = left.height();
  const auto rightHeight = right.height();

  QImage bothImage(leftWidht + rightWidht, qMax(leftHeight, rightHeight),
                   QImage::Format_RGB888);
  bothImage.fill(QColorConstants::Yellow);

  QPainter painter(&bothImage);
  painter.drawImage(0, 0, left);
  painter.drawImage(leftWidht, 0, right);

  const auto rightToBothCoord = [leftWidht](const QPoint &point) -> QPoint {
    return {point.x() + leftWidht, point.y()};
  };

  for (const auto &s : similar) {
    painter.setPen(randomColor());
    painter.drawLine(QPoint(leftFast[s.first].x(), leftFast[s.first].y()),
                     rightToBothCoord(QPoint(rightFast[s.second].x(),
                                             rightFast[s.second].y())));
  }

  bothImage.save("bothImage.jpg");

  QVector<QPair<int, int>> bestSimilar;
  timer.start();
  qDebug() << "RANSAC...";
  {
    bestSimilar = RANSAC(similar, leftBRIEF, rightBRIEF, similar.size() * 0.1,
                         2 * similar.size());

    QImage bothImage(leftWidht + rightWidht, qMax(leftHeight, rightHeight),
                     QImage::Format_RGB888);
    bothImage.fill(QColorConstants::Yellow);

    QPainter painter(&bothImage);
    painter.drawImage(0, 0, left);
    painter.drawImage(leftWidht, 0, right);

    const auto rightToBothCoord = [leftWidht](const QPoint &point) -> QPoint {
      return {point.x() + leftWidht, point.y()};
    };

    for (const auto &s : bestSimilar) {
      painter.setPen(randomColor());
      painter.drawLine(QPoint(leftFast[s.first].x(), leftFast[s.first].y()),
                       rightToBothCoord(QPoint(rightFast[s.second].x(),
                                               rightFast[s.second].y())));
    }

    bothImage.save("bestSimilarBothImage.jpg");
  }
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  //  QImage right("right.jpg");
  //  QImage left("left.jpg");
  //  // делается чтобы видеть результат на монохромных картинках
  //  right.convertTo(QImage::Format_RGB888);
  //  left.convertTo(QImage::Format_RGB888);

  //  ImageCombiner combiner;
  //  combiner.combine(left, right);

  return 0;
}
