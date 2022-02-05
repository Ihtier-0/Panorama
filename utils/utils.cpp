#include "utils.h"

#include <QRandomGenerator>

QColor randomColor() {
  const auto random = QRandomGenerator::global();
  return QColor(random->bounded(256), random->bounded(256),
                random->bounded(256));
}
