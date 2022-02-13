#ifndef TESTS_H
#define TESTS_H

#include "../../testUtils/testUtils.h"

class SteepestDescentMethodTests : public QObject {
  Q_OBJECT

private slots:
  void SteepestDescentMethodTest1();
};

DECLARE_TEST(SteepestDescentMethodTests)

#endif // TESTS_H
