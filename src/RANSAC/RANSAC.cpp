#include "RANSAC.h"

#include <QRandomGenerator>
#include <QVector>

// -----------------------------------------------------------------------------

QVector<QPair<int, int>> RANSAC(const QVector<QPair<int, int>> &similar,
                                const QVector<QBitArray> &leftBRIEF,
                                const QVector<QBitArray> &rightBRIEF,
                                const quint32 &resultCount,
                                const quint32 &iteration) {
  const int size = similar.size();

  const auto randomIndex = [&]() {
    return QRandomGenerator::global()->bounded(size);
  };

  const auto randomIndexes = [&]() {
    QVector<int> indexes;
    for (quint32 i = 0; i < resultCount; ++i) {
      indexes.push_back(randomIndex());
    }
    return indexes;
  };

  const auto solveQuality = [&](const QVector<int> &step) {
    int quality = 0;
    QPair<int, int> pair;
    for (const auto &element : step) {
      pair = similar[element];
      quality += (leftBRIEF[pair.first] ^ rightBRIEF[pair.second]).count(false);
    }
    return quality;
  };

  QVector<int> step, best = randomIndexes();
  int currentQuality, bestQuality = solveQuality(best);

  for (quint32 i = 0; i < iteration - 1; ++i) {
    step = randomIndexes();
    currentQuality = solveQuality(step);
    if (currentQuality > bestQuality) {
      best = step;
      bestQuality = currentQuality;
    }
  }

  QVector<QPair<int, int>> bestPairs;

  for (const auto &b : best) {
    bestPairs.push_back(similar[b]);
  }

  return bestPairs;
}

// -----------------------------------------------------------------------------
