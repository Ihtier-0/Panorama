#ifndef ALIASES_H
#define ALIASES_H

#include <QVector>

template <class T> using Row = QVector<T>;
template <class T> using Matrix = QVector<Row<T>>;

#endif // ALIASES_H
