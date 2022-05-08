#include "BRIEF.h"

#include <QRandomGenerator>

// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------

QBitArray BRIEF(const Matrix<float> &aBrightness, const int &aX, const int &aY,
                const QVector<QPair<QPoint, QPoint>> &BRIEFSequence) {
  const auto size = BRIEFSequence.size();
  QBitArray BRIEF(size, false);
  QPoint left, right;

  for (int i = 0; i < size; ++i) {
    left = BRIEFSequence[i].first;
    right = BRIEFSequence[i].second;

    if (aBrightness[aX + left.x()][aY + left.y()] <
        aBrightness[aX + right.x()][aY + right.y()]) {
      BRIEF.setBit(i, true);
    }
  }

  return BRIEF;
}

// -----------------------------------------------------------------------------

QVector<QPair<int, int>> findSimilar(const QVector<QBitArray> &aLeft,
                                     const QVector<QBitArray> &aRight) {
  const auto leftSize = aLeft.size();
  const auto rightSize = aRight.size();

  int closestCorner;
  int minDistance;
  int distance;

  QVector<QPair<int, int>> similar;

  for (int left = 0; left < leftSize; ++left) {
    closestCorner = 0;
    minDistance = (aLeft[left] ^ aRight[closestCorner]).count(true);
    for (int right = 0; right < rightSize; ++right) {
      if (minDistance >
          (distance = (aLeft[left] ^ aRight[right]).count(true))) {
        closestCorner = right;
        minDistance = distance;
      }
    }

    QVector<QPair<int, int>>::iterator existed =
        std::find_if(similar.begin(), similar.end(),
                     [closestCorner](const QPair<int, int> &association) {
                       return association.second == closestCorner;
                     });

    if (existed != similar.end()) {
      if (minDistance < (aLeft[left] ^ aRight[existed->second]).count(true)) {
        similar.erase(existed);
        similar.push_back({left, closestCorner});
      }
    } else {
      similar.push_back({left, closestCorner});
    }
  }

  return similar;
}

// -----------------------------------------------------------------------------
