#include "RANSAC.h"

#include <QRandomGenerator>
#include <QVector>

QVector<QPair<int, int>> RANSAC(const QVector<QPair<int, int>> &similar,
                                const QVector<QBitArray> &leftBRIEF,
                                const QVector<QBitArray> &rightBRIEF,
                                const quint32 &resultCount,
                                const quint32 &iteration) {
  const int size = similar.size();

  const auto randomIndex = [&]() {
    return QRandomGenerator::global()->bounded(size);
  };

  const auto randomElements = [&]() {
    QVector<QPair<int, int>> result;
    for (quint32 i = 0; i < resultCount; ++i) {
      result.push_back(similar[randomIndex()]);
    }
    return result;
  };

  const auto solveQuality = [&](const QVector<QPair<int, int>> &step) {
    int quality = 0;
    for (const auto &element : step) {
      quality +=
          (leftBRIEF[element.first] ^ rightBRIEF[element.second]).count(false);
    }
    return quality;
  };

  QVector<QPair<int, int>> step, best = randomElements();
  int currentQuality, bestQuality = solveQuality(best);

  for (quint32 i = 0; i < iteration - 1; ++i) {
    step = randomElements();
    currentQuality = solveQuality(step);
    if (currentQuality > bestQuality) {
      best = step;
      bestQuality = currentQuality;
    }
  }

  // с одной стророны так метод возвращает не то количество,
  // с другой точность куда лучше
  // пусть пока так
  int avg = solveQuality(best) / resultCount;

  QVector<int> toDelete;

  for (quint32 i = 0; i < resultCount; ++i) {
    if ((leftBRIEF[best[i].first] ^ rightBRIEF[best[i].second]).count(false) <
        avg) {
      toDelete.push_back(i);
    }
  }

  for (quint32 i = 0; i < toDelete.size(); ++i) {
    best.remove(toDelete[i] - i);
  }

  return best;
}
