#ifndef TESTS_H
#define TESTS_H

#include "../../testUtils/testUtils.h"

class GradientDescentTests : public QObject {
  Q_OBJECT

private slots:
  void GradientTest1();

  void GradientDescentTest1();
};

DECLARE_TEST(GradientDescentTests)

#endif // TESTS_H
