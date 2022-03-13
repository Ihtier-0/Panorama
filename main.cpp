#include <QCoreApplication>

#include <QElapsedTimer>
#include <QPainter>

#include "BRIEF/BRIEF.h"
#include "Combine/Combine.h"
#include "FAST/FAST.h"
#include "RANSAC/RANSAC.h"
#include "blur/blur.h"
#include "utils/MatrixUtils.h"
#include "utils/YUVUtils.h"
#include "utils/utils.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QElapsedTimer timer;

  QImage left("rio-00.png");
  QImage right("rio-01.png");
  Matrix<qreal> leftBlur, rightBlur;
  // у векторов fast и brief одинаковый размер
  QVector<QVector2D> leftFast, rightFast;
  QVector<QBitArray> leftBRIEF, rightBRIEF;
  QVector<QPair<int, int>> similar;
  QVector<QPair<int, int>> bestSimilar;
  qreal fastT = 20;

  const auto leftWidht = left.width();
  const auto rightWidht = right.width();

  const auto leftHeight = left.height();
  const auto rightHeight = right.height();

  const auto rightToBothCoord = [leftWidht](const QPoint &point) -> QPoint {
    return {point.x() + leftWidht, point.y()};
  };
  // FAST left
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

    auto leftCopy = left;

    drawOverlay(overlay, leftCopy);

    leftCopy.save("3) left overlay.jpg");
  }

  // FAST right
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

    auto rightCopy = right;

    drawOverlay(overlay, rightCopy);

    rightCopy.save("3) right overlay.jpg");
  }

  // BRIEF
  {
    const auto sequence = BRIEFSequence();

    timer.start();
    qDebug() << "BRIEF...";
    for (const auto left : leftFast) {
      leftBRIEF.push_back(BRIEF(leftBlur, left.x(), left.y(), sequence));
    }

    for (const auto right : rightFast) {
      rightBRIEF.push_back(BRIEF(rightBlur, right.x(), right.y(), sequence));
    }
    qDebug() << "DONE!" << timer.elapsed() << "ms";

    similar = findSimilar(leftBRIEF, rightBRIEF);

    QImage bothImage(leftWidht + rightWidht, qMax(leftHeight, rightHeight),
                     QImage::Format_RGB888);
    bothImage.fill(QColorConstants::Yellow);

    QPainter painter(&bothImage);
    painter.drawImage(0, 0, left);
    painter.drawImage(leftWidht, 0, right);

    for (const auto &s : similar) {
      painter.setPen(randomColor());
      painter.drawLine(QPoint(leftFast[s.first].x(), leftFast[s.first].y()),
                       rightToBothCoord(QPoint(rightFast[s.second].x(),
                                               rightFast[s.second].y())));
    }

    bothImage.save("bothImage.jpg");
  }

  // RANSAC
  {
    timer.start();
    qDebug() << "RANSAC...";

    bestSimilar = RANSAC(similar, leftBRIEF, rightBRIEF, similar.size() * 0.15,
                         2 * similar.size());

    QImage bothImage(leftWidht + rightWidht, qMax(leftHeight, rightHeight),
                     QImage::Format_RGB888);
    bothImage.fill(QColorConstants::Yellow);

    QPainter painter(&bothImage);
    painter.drawImage(0, 0, left);
    painter.drawImage(leftWidht, 0, right);

    for (const auto &s : bestSimilar) {
      painter.setPen(randomColor());
      painter.drawLine(QPoint(leftFast[s.first].x(), leftFast[s.first].y()),
                       rightToBothCoord(QPoint(rightFast[s.second].x(),
                                               rightFast[s.second].y())));
    }

    bothImage.save("bestSimilarBothImage.jpg");
    qDebug() << "DONE!" << timer.elapsed() << "ms";
  }

  // Combine
  {
    const auto initial =
        initialApproximation(leftFast, rightFast, leftBRIEF, rightBRIEF,
                             bestSimilar, rightToBothCoord);
    QImage bothImage(leftWidht + rightWidht - qAbs(initial.x()),
                     qMax(leftHeight, rightHeight) + qAbs(initial.y()),
                     QImage::Format_RGB888);
    QImage bothSimilar(leftWidht + rightWidht - qAbs(initial.x()),
                       qMax(leftHeight, rightHeight) + qAbs(initial.y()),
                       QImage::Format_RGB888);
    bothSimilar.fill(QColorConstants::Yellow);
    bothImage.fill(QColorConstants::Yellow);

    QPainter painter(&bothImage);
    painter.drawImage(0, qAbs(initial.y()), left);
    painter.drawImage(rightToBothCoord({0, 0}).x() + initial.x(),
                      rightToBothCoord({0, 0}).y() + initial.y(), right);
    bothImage.save("initialApproximation.jpg");

    QPainter painterSimilar(&bothSimilar);
    for (const auto &s : bestSimilar) {
      painterSimilar.setPen(randomColor());
      painterSimilar.drawLine(
          QPoint(leftFast[s.first].x(), leftFast[s.first].y()),
          rightToBothCoord(
              QPoint(rightFast[s.second].x(), rightFast[s.second].y())) +
              initial.toPoint());
    }
    bothSimilar.save("initialApproximationLines.jpg");
  }

  return 0;
}
