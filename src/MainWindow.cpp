#include "MainWindow.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QPainter>

#include "BRIEF/BRIEF.h"
#include "FAST/FAST.h"
#include "RANSAC/RANSAC.h"
#include "blur/blur.h"

// -----------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  if (!mLeft.load("rio-00.png")) {
    qDebug() << "can't load left image";
  }
  if (!mRight.load("rio-01.png")) {
    qDebug() << "can't load right image";
  }
  mLeft.convertTo(QImage::Format_RGB888);
  mRight.convertTo(QImage::Format_RGB888);

  mBRIEFSequence = BRIEFSequence();

  auto left = beforeCombine(mLeft, "left");
  qDebug();
  auto right = beforeCombine(mRight, "right");
  qDebug();

  auto similar = findSimilar(left, right);

  auto bestSimilar = avgFilter(left.second, right.second, similar);
  if (mDebug) {
    const auto leftWidht = mLeft.width();
    const auto rightWidht = mRight.width();

    const auto leftHeight = mLeft.height();
    const auto rightHeight = mRight.height();

    const auto rightToBothCoord = [leftWidht](const QPoint &point) -> QPoint {
      return {point.x() + leftWidht, point.y()};
    };
    QImage bothImage(leftWidht + rightWidht, qMax(leftHeight, rightHeight),
                     QImage::Format_RGB888);
    bothImage.fill(QColorConstants::Yellow);

    QPainter painter(&bothImage);
    painter.drawImage(0, 0, mLeft);
    painter.drawImage(leftWidht, 0, mRight);

    for (const auto &s : bestSimilar) {
      painter.setPen(randomColor());
      painter.drawLine(QPoint(left.first[s.first].x(), left.first[s.first].y()),
                       rightToBothCoord(QPoint(right.first[s.second].x(),
                                               right.first[s.second].y())));
    }

    bothImage.save("avg filter similar.jpg");
  }
}

// -----------------------------------------------------------------------------

MainWindow::~MainWindow() {}

// -----------------------------------------------------------------------------

QPair<MainWindow::fastInfo, MainWindow::briefInfo>
MainWindow::beforeCombine(const QImage &image, const QString name) {
  qDebug() << name << "beforeCombine begin";
  QElapsedTimer timer;

  qDebug() << name << "brightness...";
  timer.start();
  Matrix<float> brightness = YUV::getBrightness(image, mRecommendation);
  if (mDebug) {
    toImage(brightness).save(name + " brightness.jpg");
  }
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  qDebug() << name << "blur...";
  timer.start();
  Matrix<float> blur = blur::gaussian(brightness, mStandardDeviation);
  if (mDebug) {
    toImage(blur).save(name + " blur.jpg");
  }
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  qDebug() << name << "FAST...";
  timer.start();
  auto fast = FAST(blur, mThreshold);
  if (mDebug) {
    auto fastImage = image;

    for (const auto &f : fast) {
      drawSquare(fastImage, f.x(), f.y(), randomColor());
    }
    fastImage.save(name + " fast.jpg");
  }
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  qDebug() << name << "BRIEF...";
  timer.start();
  QVector<QBitArray> brief;
  for (const auto &f : fast) {
    brief.push_back(BRIEF(brightness, f.x(), f.y(), mBRIEFSequence));
  }
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  qDebug() << name << "beforeCombine end";

  return {fast, brief};
}

// -----------------------------------------------------------------------------

QVector<QPair<int, int>>
MainWindow::findSimilar(const QPair<fastInfo, briefInfo> &left,
                        const QPair<fastInfo, briefInfo> &right) {
  QElapsedTimer timer;
  qDebug() << "find similar...";
  timer.start();
  QVector<QPair<int, int>> similar = ::findSimilar(left.second, right.second);
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  if (mDebug) {
    const auto leftWidht = mLeft.width();
    const auto rightWidht = mRight.width();

    const auto leftHeight = mLeft.height();
    const auto rightHeight = mRight.height();

    const auto rightToBothCoord = [leftWidht](const QPoint &point) -> QPoint {
      return {point.x() + leftWidht, point.y()};
    };
    QImage bothImage(leftWidht + rightWidht, qMax(leftHeight, rightHeight),
                     QImage::Format_RGB888);
    bothImage.fill(QColorConstants::Yellow);

    QPainter painter(&bothImage);
    painter.drawImage(0, 0, mLeft);
    painter.drawImage(leftWidht, 0, mRight);

    for (const auto &s : similar) {
      painter.setPen(randomColor());
      painter.drawLine(QPoint(left.first[s.first].x(), left.first[s.first].y()),
                       rightToBothCoord(QPoint(right.first[s.second].x(),
                                               right.first[s.second].y())));
    }

    bothImage.save("similar.jpg");
  }

  qDebug() << "RANSAC...";
  timer.start();
  QVector<QPair<int, int>> bestSimilar =
      RANSAC(similar, left.second, right.second, mResultCount, mIteration);
  qDebug() << "DONE!" << timer.elapsed() << "ms";

  if (mDebug) {
    const auto leftWidht = mLeft.width();
    const auto rightWidht = mRight.width();

    const auto leftHeight = mLeft.height();
    const auto rightHeight = mRight.height();

    const auto rightToBothCoord = [leftWidht](const QPoint &point) -> QPoint {
      return {point.x() + leftWidht, point.y()};
    };
    QImage bothImage(leftWidht + rightWidht, qMax(leftHeight, rightHeight),
                     QImage::Format_RGB888);
    bothImage.fill(QColorConstants::Yellow);

    QPainter painter(&bothImage);
    painter.drawImage(0, 0, mLeft);
    painter.drawImage(leftWidht, 0, mRight);

    for (const auto &s : bestSimilar) {
      painter.setPen(randomColor());
      painter.drawLine(QPoint(left.first[s.first].x(), left.first[s.first].y()),
                       rightToBothCoord(QPoint(right.first[s.second].x(),
                                               right.first[s.second].y())));
    }

    bothImage.save("best similar.jpg");
  }

  return bestSimilar;
}

// -----------------------------------------------------------------------------

QVector<QPair<int, int>>
MainWindow::avgFilter(const briefInfo &left, const briefInfo &right,
                      const QVector<QPair<int, int>> &similar) {
  const auto solveQuality = [&](const QVector<QPair<int, int>> &step) {
    int quality = 0;
    for (const auto &element : step) {
      quality += (left[element.first] ^ right[element.second]).count(false);
    }
    return quality;
  };

  const auto size = similar.size();
  int avg = solveQuality(similar) / size;

  QVector<int> toDelete;

  for (quint32 i = 0; i < size; ++i) {
    if ((left[similar[i].first] ^ right[similar[i].second]).count(false) <
        avg) {
      toDelete.push_back(i);
    }
  }

  auto filtered = similar;

  for (quint32 i = 0; i < toDelete.size(); ++i) {
    filtered.remove(toDelete[i] - i);
  }

  return filtered;
}

// -----------------------------------------------------------------------------

float MainWindow::getStandardDeviation() const { return mStandardDeviation; }

void MainWindow::setStandardDeviation(float newStandardDeviation) {
  mStandardDeviation = newStandardDeviation;
}

// -----------------------------------------------------------------------------

bool MainWindow::getDebug() const { return mDebug; }

void MainWindow::setDebug(bool newDebug) { mDebug = newDebug; }

// -----------------------------------------------------------------------------

YUV::Recommendation MainWindow::getRecommendation() const {
  return mRecommendation;
}

void MainWindow::setRecommendation(YUV::Recommendation newRecommendation) {
  mRecommendation = newRecommendation;
}

// -----------------------------------------------------------------------------
