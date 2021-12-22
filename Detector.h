#ifndef DETECTOR_H
#define DETECTOR_H

#include <QImage>

template <class T> using Matrix = QVector<QVector<T>>;

template <class T> using Row = QVector<T>;

class Detector {
public:
  Detector(const QSize &tileSize, const qreal &radius,
           const qreal standardDeviation);

  QImage detecting(const QImage &image);

  static qreal getYUVBrightness(const QColor &color);
  static Matrix<qreal> getYUVBrightness(const QImage &image);

  static QImage matrixToImage(const Matrix<qreal> &matrix);

  static qreal gaussianBlur(const Matrix<qreal> &matrix, const qreal &radius,
                            const qreal &standardDeviation, const int &x,
                            const int &y);
  static Matrix<qreal> gaussianBlur(const Matrix<qreal> &matrix,
                                    const qreal &radius,
                                    const qreal &standardDeviation);

private:
  QSize m_tileSize;
  qreal m_radius;
  qreal m_standardDeviation;
};

#endif // DETECTOR_H
