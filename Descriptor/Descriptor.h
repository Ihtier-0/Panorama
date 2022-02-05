#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "Direction/Direction.h"

struct Descriptor {
  Direction::Direction direction;
  qreal probability;
  QPoint position;
};

QDebug operator<<(QDebug out, const Descriptor &descriptor);

#endif // DESCRIPTOR_H
