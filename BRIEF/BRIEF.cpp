#include "BRIEF.h"

#include <QRandomGenerator>

QVector<QPair<QPoint, QPoint>> BRIEFSequence() {
  // может быть изменить генератор
  auto random = QRandomGenerator::global();

  QVector<QPair<QPoint, QPoint>> sequence;

  for (int i = 0; i < 256; ++i) {
    // random in [-15, 16)
    sequence.push_back({{random->bounded(-15, 16), random->bounded(-15, 16)},
                        {random->bounded(-15, 16), random->bounded(-15, 16)}});
  }

  return sequence;
}

QBitArray BRIEF(const Matrix<qreal> &brightness, const int &x, const int &y,
                const QVector<QPair<QPoint, QPoint>> &BRIEFSequence) {
  const auto size = BRIEFSequence.size();
  QBitArray BRIEF(size, false);
  QPoint left, right;

  for (int i = 0; i < size; ++i) {
    left = BRIEFSequence[i].first;
    right = BRIEFSequence[i].second;

    if (brightness[x + left.x()][y + left.y()] <
        brightness[x + right.x()][y + right.y()]) {
      BRIEF.setBit(i, true);
    }
  }

  return BRIEF;
}

QVector<QPair<int, int>> findSimilar(const QVector<QBitArray> &leftBRIEF,
                                     const QVector<QBitArray> &rightBRIEF) {
  const auto leftSize = leftBRIEF.size();
  const auto rightSize = rightBRIEF.size();

  int closestCorner;
  int minDistance;
  int distance;

  QVector<QPair<int, int>> similar;

  for (int left = 0; left < leftSize; ++left) {
    closestCorner = 0;
    minDistance = (leftBRIEF[left] ^ rightBRIEF[closestCorner]).count(true);
    for (int right = 0; right < rightSize; ++right) {
      if (minDistance >
          (distance = (leftBRIEF[left] ^ rightBRIEF[right]).count(true))) {
        closestCorner = right;
        minDistance = distance;
      }
    }

    const auto existed =
        std::find_if(similar.begin(), similar.end(),
                     [closestCorner](const QPair<int, int> &association) {
                       return association.second == closestCorner;
                     });

    if (existed != similar.end()) {
      if (minDistance <
          (leftBRIEF[left] ^ rightBRIEF[existed->second]).count(true)) {
        similar.erase(existed);
        similar.push_back({left, closestCorner});
      }
    } else {
      similar.push_back({left, closestCorner});
    }
  }

  return similar;
}
