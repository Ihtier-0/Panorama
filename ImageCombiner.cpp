#include "ImageCombiner.h"

#include <QDebug>
#include <QPainter>
#include <QTransform>
#include <QVector2D>

#include "Detector/Detector.h"
#include "Optimization/GradientDescent/GradientDescent.h"
#include "Optimization/SteepestDescentMethod/SteepestDescentMethod.h"
#include "utils/utils.h"

static QVector<QPair<int, int>>
findAssociations(const QVector<Descriptor> &leftCorners,
                 const QVector<Descriptor> &rightCorners) {
  QVector<QPair<int, int>> associations;

  const auto rightCornersCount = rightCorners.size();
  const auto leftCornersCount = leftCorners.size();

  int closestCorner;
  qreal minDistance;
  qreal distance;

  // одного цикла достаточно,
  // потому что если точки не соединены
  // то и в обратном цикле они вряд-ли соединятся
  for (int left = 0; left < leftCornersCount; ++left) {
    closestCorner = 0;
    minDistance = qAbs(leftCorners[left].probability -
                       rightCorners[closestCorner].probability);
    for (int right = 0; right < rightCornersCount; ++right) {
      if (minDistance > (distance = qAbs(leftCorners[left].probability -
                                         rightCorners[right].probability))) {
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
      if (qAbs(leftCorners[left].probability -
               rightCorners[closestCorner].probability) <
          qAbs(leftCorners[existed->first].probability -
               rightCorners[closestCorner].probability)) {
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
                              const QVector<Descriptor> &leftCorners,
                              const QVector<Descriptor> &rightCorners,
                              const QTransform &transform) {
  qreal loss = 0;
  qreal difference;

  for (const auto &association : associations) {
    difference =
        QVector2D(
            QPointF(leftCorners[association.first].position) -
            transform.map(QPointF(rightCorners[association.second].position)))
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

  const auto rightCorners = detector.detecting(right, &rightResult);
  const auto leftCorners = detector.detecting(left, &leftResult);
  qDebug() << "end";

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
      findAssociations(leftCorners, rightCorners);

  const auto rightToBothCoord = [leftWidht](const QPoint &point) -> QPoint {
    return {point.x() + leftWidht, point.y()};
  };

  for (const auto &association : associations) {
    painter.setPen(randomColor());
    painter.drawLine(
        leftCorners[association.first].position,
        rightToBothCoord(rightCorners[association.second].position));
  }

  bothImage.save("20) bothAssociations.jpg");

  const auto best = bestPair(associations, leftCorners, rightCorners);
  const auto move = rightCorners[associations[best].second].position -
                    leftCorners[associations[best].first].position;

  QImage initialApproximation(leftWidht + rightWidht,
                              qMax(leftHeight, rightHeight),
                              QImage::Format_RGB888);
  QPainter painter2(&initialApproximation);
  painter2.drawImage(0, 0, leftResult);
  painter2.drawImage(leftWidht + move.x(), 0 + move.y(), rightResult);
  initialApproximation.save("30) initialApproximation.jpg");
}

int ImageCombiner::bestPair(const QVector<QPair<int, int>> &associations,
                            const QVector<Descriptor> &leftCorners,
                            const QVector<Descriptor> &rightCorners) {
  int best = 0;
  qreal bestProbability =
      qAbs(leftCorners[associations[best].first].probability -
           rightCorners[associations[best].second].probability);
  qreal currentProbability;

  const auto size = associations.size();

  for (int i = 0; i < size; ++i) {
    if (bestProbability >
        (currentProbability =
             qAbs(leftCorners[associations[i].first].probability -
                  rightCorners[associations[i].second].probability))) {
      bestProbability = currentProbability;
      best = i;
    }
  }

  return best;
}
