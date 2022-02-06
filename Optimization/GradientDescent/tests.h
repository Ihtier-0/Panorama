#ifndef TESTS_H
#define TESTS_H

#include "../../testUtils/testUtils.h"

class tests : public QObject {
  Q_OBJECT

private slots:
  void test0();
};

DECLARE_TEST(tests)

#endif // TESTS_H
