#include "ImageCombiner.h"

#include <QDebug>
#include <QPainter>
#include <QTransform>
#include <QVector2D>

#include "Detector/Detector.h"
#include "utils/utils.h"

static QVector<QPair<int, int>>
findAssociations(const QVector<Descriptor> &leftСorners,
                 const QVector<Descriptor> &rightСorners) {
  QVector<QPair<int, int>> associations;

  const auto rightСornersCount = rightСorners.size();
  const auto leftCornersCount = leftСorners.size();

  int closestCorner;
  qreal minDistance;
  qreal distance;

  // одного цикла достаточно,
  // потому что если точки не соединены
  // то и в обратном цикле они вряд-ли соединятся
  for (int left = 0; left < leftCornersCount; ++left) {
    closestCorner = 0;
    minDistance = qAbs(leftСorners[left].probability -
                       rightСorners[closestCorner].probability);
    for (int right = 0; right < rightСornersCount; ++right) {
      if (minDistance > (distance = qAbs(leftСorners[left].probability -
                                         rightСorners[right].probability))) {
        closestCorner = right;
        minDistance = distance;
      }
    }
    const auto existed =
        std::find_if(associations.begin(), associations.end(),
                     [closestCorner](const QPair<int, int> &association) {
                       return association.second == closestCorner;
                     });

    if (existed != associations.end()) {
      if (qAbs(leftСorners[left].probability -
               rightСorners[closestCorner].probability) <
          qAbs(leftСorners[existed->first].probability -
               rightСorners[closestCorner].probability)) {
        associations.erase(existed);
        associations.push_back({left, closestCorner});
      }
    } else {
      associations.push_back({left, closestCorner});
    }
  }

  return associations;
}

static qreal associationsLoss(const QVector<QPair<int, int>> &associations,
                              const QVector<Descriptor> &leftСorners,
                              const QVector<Descriptor> &rightСorners,
                              const QTransform &transform) {
  qreal loss = 0;
  qreal difference;

  for (const auto &association : associations) {
    difference =
        QVector2D(transform.map(leftСorners[association.first].position) -
                  transform.map(rightСorners[association.second].position))
            .length();
    loss += difference * difference;
  }

  return loss;
}

void ImageCombiner::combine(const QImage &left, const QImage &right) {
  Detector detector(3, 5, 1,
                    Detector::clippingMode::reducedEdgeMaxApproximation);

  QImage rightResult;
  QImage leftResult;

  const auto rightСorners = detector.detecting(right, &rightResult);
  const auto leftСorners = detector.detecting(left, &leftResult);

  const auto leftWidht = left.width();
  const auto rightWidht = right.width();

  const auto leftHeight = left.height();
  const auto rightHeight = right.height();

  QImage bothImage(leftWidht + rightWidht, qMax(leftHeight, rightHeight),
                   QImage::Format_RGB888);
  bothImage.fill(QColorConstants::Yellow);

  QPainter painter(&bothImage);
  painter.drawImage(0, 0, leftResult);
  painter.drawImage(leftWidht, 0, rightResult);

  bothImage.save("10) bothResult.jpg");

  QVector<QPair<int, int>> associations =
      findAssociations(leftСorners, rightСorners);

  const auto rightToBothCoord = [leftWidht](const QPoint &point) -> QPoint {
    return {point.x() + leftWidht, point.y()};
  };

  for (const auto &association : associations) {
    painter.setPen(randomColor());
    painter.drawLine(
        leftСorners[association.first].position,
        rightToBothCoord(rightСorners[association.second].position));
  }

  bothImage.save("20) bothAssociations.jpg");

  QTransform transform;
}
