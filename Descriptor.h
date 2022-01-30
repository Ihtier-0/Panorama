#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "Direction/Direction.h"

struct Descriptor {
  Direction::Direction direction;
  qreal probability;
  QPoint position;
};

#endif // DESCRIPTOR_H
