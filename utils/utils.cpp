#include "utils.h"

#include <QRandomGenerator>

QColor randomColor() {
  const auto random = QRandomGenerator::global();
  return QColor(random->bounded(256), random->bounded(256),
                random->bounded(256));
}

QTransform operator+(const QTransform &left, const QTransform &right) {
  // clang-format off
  return QTransform(left.m11() + right.m11(),
                    left.m12() + right.m12(),
                    left.m13() + right.m13(),

                    left.m21() + right.m21(),
                    left.m22() + right.m22(),
                    left.m23() + right.m23(),

                    left.m31() + right.m31(),
                    left.m32() + right.m32(),
                    left.m33() + right.m33());
  // clang-format on
}
