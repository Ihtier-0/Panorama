#include "utils.h"

#include <QRandomGenerator>
#include <QtMath>

QColor randomColor() {
  const auto random = QRandomGenerator::global();
  return QColor(random->bounded(256), random->bounded(256),
                random->bounded(256));
}

QVector<qreal> multiply(const qreal &number, const QVector<qreal> &vector) {
  const auto size = vector.size();

  QVector<qreal> result(size);

  for (int i = 0; i < size; ++i) {
    result[i] = vector[i] * number;
  }

  return result;
}

QVector<qreal> multiply(const QVector<qreal> &vector, const qreal &number) {
  return multiply(number, vector);
}

QVector<qreal> sum(const QVector<qreal> &left, const QVector<qreal> &right) {
  const auto leftSize = left.size();
  const auto rightSize = right.size();

  const auto minSize = qMin(leftSize, rightSize);

  QVector<qreal> result(minSize);

  for (int i = 0; i < minSize; ++i) {
    result[i] = left[i] + right[i];
  }

  return result;
}

qreal norm(const QVector<qreal> &vector) {
  const auto size = vector.size();

  qreal result = 0;

  for (int i = 0; i < size; ++i) {
    result += vector[i] * vector[i];
  }

  return result;
}
