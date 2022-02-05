#include "Descriptor.h"

#include <QDebug>

QDebug operator<<(QDebug out, const Descriptor &descriptor) {
  out << '{' << Direction::toPoint(descriptor.direction) << ','
      << descriptor.probability << ',' << descriptor.position << '}';

  return out;
}
