#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QTest>

namespace TestUtils {
typedef QList<QObject *> TestList;

inline TestList &testList() {
  static TestList list;
  return list;
}

inline bool findObject(QObject *object) {
  TestList &list = testList();
  if (list.contains(object)) {
    return true;
  }
  for (const QObject *test : list) {
    if (test->objectName() == object->objectName()) {
      return true;
    }
  }
  return false;
}

inline void addTest(QObject *object) {
  TestList &list = testList();
  if (!findObject(object)) {
    list.append(object);
  }
}

inline int run(int argc, char *argv[]) {
  int ret = 0;

  for (QObject *test : testList()) {
    ret += QTest::qExec(test, argc, argv);
  }

  return ret;
}
} // namespace TestUtils

template <class T> class Test {
public:
  QSharedPointer<T> child;

  Test(const QString &name) : child(new T) {
    child->setObjectName(name);
    TestUtils::addTest(child.data());
  }
};

#define DECLARE_TEST(className) static Test<className> t(#className);

int startTests(int argc, char *argv[]);

#endif // TESTUTILS_H
