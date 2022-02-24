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

void drawSquare(const int &row, const int &col, Matrix<qreal> &matrix) {
  const auto width = matrix.size();
  const auto height = matrix[0].size();

  const auto onImage = [width, height](const int &x, const int &y) -> bool {
    return x >= 0 && y >= 0 && x < width && y < height;
  };

  const auto radius = 3;

  const auto left = col - radius;
  const auto right = col + radius;

  for (int x = row - radius; x <= row + radius; ++x) {
    if (onImage(x, left)) {
      matrix[x][left] = 255;
    }
    if (onImage(x, right)) {
      matrix[x][right] = 255;
    }
  }

  const auto up = row - radius;
  const auto down = row + radius;

  for (int y = col - radius; y <= col + radius; ++y) {
    if (onImage(up, y)) {
      matrix[up][y] = 255;
    }
    if (onImage(down, y)) {
      matrix[down][y] = 255;
    }
  }
}
