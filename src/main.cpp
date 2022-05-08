#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  MainWindow m;
  m.show();

  //  // Combine
  //  {
  //    const auto initial =
  //        initialApproximation(leftFast, rightFast, leftBRIEF, rightBRIEF,
  //                             bestSimilar, rightToBothCoord);
  //    QImage bothImage(leftWidht + rightWidht - qAbs(initial.x()),
  //                     qMax(leftHeight, rightHeight) + qAbs(initial.y()),
  //                     QImage::Format_RGB888);
  //    QImage bothSimilar(leftWidht + rightWidht - qAbs(initial.x()),
  //                       qMax(leftHeight, rightHeight) + qAbs(initial.y()),
  //                       QImage::Format_RGB888);
  //    bothSimilar.fill(QColorConstants::Yellow);
  //    bothImage.fill(QColorConstants::Yellow);

  //    QPainter painter(&bothImage);
  //    painter.drawImage(0, qAbs(initial.y()), left);
  //    painter.drawImage(rightToBothCoord({0, 0}).x() + initial.x(),
  //                      rightToBothCoord({0, 0}).y() + initial.y(), right);
  //    bothImage.save("initialApproximation.jpg");

  //    QPainter painterSimilar(&bothSimilar);
  //    for (const auto &s : bestSimilar) {
  //      painterSimilar.setPen(randomColor());
  //      painterSimilar.drawLine(
  //          QPoint(leftFast[s.first].x(), leftFast[s.first].y()),
  //          rightToBothCoord(
  //              QPoint(rightFast[s.second].x(), rightFast[s.second].y())) +
  //              initial.toPoint());
  //    }
  //    bothSimilar.save("initialApproximationLines.jpg");
  //  }

  return a.exec();
}
