#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "utils/utils.h"
#include <QMainWindow>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  YUV::Recommendation getRecommendation() const;
  void setRecommendation(YUV::Recommendation newRecommendation);

  bool getDebug() const;
  void setDebug(bool newDebug);

  float getStandardDeviation() const;
  void setStandardDeviation(float newStandardDeviation);

private:
  using briefInfo = QVector<QBitArray>;
  using fastInfo = QVector<QVector2D>;
  QPair<fastInfo, briefInfo> beforeCombine(const QImage &image,
                                           const QString name);
  QVector<QPair<int, int>> findSimilar(const QPair<fastInfo, briefInfo> &left,
                                       const QPair<fastInfo, briefInfo> &right);
  QVector<QPair<int, int>> avgFilter(const briefInfo &left,
                                     const briefInfo &right,
                                     const QVector<QPair<int, int>> &similar);

  YUV::Recommendation mRecommendation = YUV::Recommendation::BT601;
  float mStandardDeviation = 2;
  float mThreshold = 10;
  bool mDebug = true;
  quint32 mResultCount = 15;
  quint32 mIteration = 1000;

  QImage mLeft;
  QImage mRight;

  QVector<QPair<QPoint, QPoint>> mBRIEFSequence;
};
#endif // MAINWINDOW_H
