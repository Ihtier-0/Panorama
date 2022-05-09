#include "MainWindow.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>

#include "BRIEF/BRIEF.h"
#include "FAST/FAST.h"
#include "RANSAC/RANSAC.h"
#include "blur/blur.h"

// -----------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  //  if (!mLeft.load("rio-00.png")) {
  //    qDebug() << "can't load left image";
  //  }
  //  if (!mRight.load("rio-01.png")) {
  //    qDebug() << "can't load right image";
  //  }
  //  mLeft.convertTo(QImage::Format_RGB888);
  //  mRight.convertTo(QImage::Format_RGB888);

  //  mBRIEFSequence = BRIEFSequence();

  //  auto left = beforeCombine(mLeft, "left");
  //  qDebug();
  //  auto right = beforeCombine(mRight, "right");
  //  qDebug();

  //  auto similar = findSimilar(left, right);

  //  if (mAvgFilther) {
  //    similar = avgFilter(left.second, right.second, similar);
  //    if (mDebug) {
  //      const auto leftWidht = mLeft.width();
  //      const auto rightWidht = mRight.width();

  //      const auto leftHeight = mLeft.height();
  //      const auto rightHeight = mRight.height();

  //      const auto rightToBothCoord = [leftWidht](const QPoint &point) ->
  //      QPoint {
  //        return {point.x() + leftWidht, point.y()};
  //      };
  //      QImage bothImage(leftWidht + rightWidht, qMax(leftHeight,
  //      rightHeight),
  //                       QImage::Format_RGB888);
  //      bothImage.fill(QColorConstants::Yellow);

  //      QPainter painter(&bothImage);
  //      painter.drawImage(0, 0, mLeft);
  //      painter.drawImage(leftWidht, 0, mRight);

  //      for (const auto &s : similar) {
  //        painter.setPen(randomColor());
  //        painter.drawLine(
  //            QPoint(left.first[s.first].x(), left.first[s.first].y()),
  //            rightToBothCoord(
  //                QPoint(right.first[s.second].x(),
  //                right.first[s.second].y())));
  //      }

  //      bothImage.save("avg filter similar.jpg");
  //    }
  //  }
  this->createLayout();
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

void MainWindow::recommendationChanged(const QString &text) {
  qDebug() << "recommendationChanged" << text;
  if (text == "BT601") {
    qDebug() << "\tBT601";
    mRecommendation = YUV::Recommendation::BT601;
  } else if (text == "BT709") {
    qDebug() << "\tBT709";
    mRecommendation = YUV::Recommendation::BT709;
  } else if (text == "BT2020") {
    qDebug() << "\tBT2020";
    mRecommendation = YUV::Recommendation::BT2020;
  } else {
    qDebug() << "\tunknown";
  }
}

void MainWindow::thresholdChanged(double d) {
  qDebug() << "thresholdChanged" << d;
  mThreshold = d;
}

void MainWindow::standardDeviationChanged(double d) {
  qDebug() << "standardDeviationChanged" << d;
  mStandardDeviation = d;
}

void MainWindow::debugChanged(int state) {
  qDebug() << "debugChanged" << state;
  mDebug = Qt::Checked == state;
}

void MainWindow::resultCountChanged(double d) {
  qDebug() << "resultCountChanged" << d;
  mResultCount = d;
}

void MainWindow::iterationChanged(double d) {
  qDebug() << "iterationChanged" << d;
  mIteration = d;
}

void MainWindow::avgFiltherChanged(int state) {
  qDebug() << "avgFiltherChanged" << state;
  mAvgFilther = Qt::Checked == state;
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

void MainWindow::createLayout() {
  auto widget = new QWidget;

  auto main = new QHBoxLayout;
  {
    auto firstImageVBox = new QVBoxLayout;
    { firstImageVBox->addWidget(new QLabel("first image")); }
    main->addLayout(firstImageVBox);

    auto secondImageVBox = new QVBoxLayout;
    { secondImageVBox->addWidget(new QLabel("second image")); }
    main->addLayout(secondImageVBox);

    auto buttons = new QVBoxLayout;
    {
      auto recommendationsHBox = new QHBoxLayout;
      {
        auto recommendations = new QComboBox;
        {
          recommendations->addItem("BT601");
          recommendations->addItem("BT709");
          recommendations->addItem("BT2020");
          recommendations->setCurrentIndex((int)mRecommendation);
          connect(recommendations, &QComboBox::currentTextChanged, this,
                  &MainWindow::recommendationChanged);
        }
        recommendationsHBox->addWidget(new QLabel("recommendation:"));
        recommendationsHBox->addWidget(recommendations);
      }
      buttons->addLayout(recommendationsHBox);

      auto standardDeviationHBox = new QHBoxLayout;
      {
        auto standardDeviationSpin = new QDoubleSpinBox;
        {
          standardDeviationSpin->setValue(mStandardDeviation);
          standardDeviationSpin->setSingleStep(0.01);
          standardDeviationSpin->setMinimum(0.01);
          standardDeviationSpin->setMaximum(std::numeric_limits<double>::max());
          connect(standardDeviationSpin, &QDoubleSpinBox::valueChanged, this,
                  &MainWindow::standardDeviationChanged);
        }
        standardDeviationHBox->addWidget(new QLabel("standardDeviation:"));
        standardDeviationHBox->addWidget(standardDeviationSpin);
      }
      buttons->addLayout(standardDeviationHBox);

      auto thresholdHBox = new QHBoxLayout;
      {
        auto thresholdSpin = new QDoubleSpinBox;
        {
          thresholdSpin->setValue(mThreshold);
          thresholdSpin->setSingleStep(0.01);
          thresholdSpin->setMinimum(0);
          thresholdSpin->setMaximum(std::numeric_limits<double>::max());
          connect(thresholdSpin, &QDoubleSpinBox::valueChanged, this,
                  &MainWindow::thresholdChanged);
        }
        thresholdHBox->addWidget(new QLabel("threshold:"));
        thresholdHBox->addWidget(thresholdSpin);
      }
      buttons->addLayout(thresholdHBox);

      auto debugHBox = new QHBoxLayout;
      {
        auto debugBox = new QCheckBox;
        {
          if (mDebug) {
            debugBox->setCheckState(Qt::CheckState::Checked);
          } else {
            debugBox->setCheckState(Qt::CheckState::Unchecked);
          }
          connect(debugBox, &QCheckBox::stateChanged, this,
                  &MainWindow::debugChanged);
        }
        debugHBox->addWidget(new QLabel("debug:"));
        debugHBox->addWidget(debugBox);
      }
      buttons->addLayout(debugHBox);

      auto resultCountHBox = new QHBoxLayout;
      {
        auto resultCountSpin = new QDoubleSpinBox;
        {
          resultCountSpin->setDecimals(0);
          resultCountSpin->setSingleStep(1);
          resultCountSpin->setMinimum(1);
          resultCountSpin->setValue(mResultCount);
          resultCountSpin->setMaximum(std::numeric_limits<double>::max());
          connect(resultCountSpin, &QDoubleSpinBox::valueChanged, this,
                  &MainWindow::resultCountChanged);
        }
        resultCountHBox->addWidget(new QLabel("RANSAC resultCount:"));
        resultCountHBox->addWidget(resultCountSpin);
      }
      buttons->addLayout(resultCountHBox);

      auto iterationHBox = new QHBoxLayout;
      {
        auto iterationSpin = new QDoubleSpinBox;
        {
          iterationSpin->setDecimals(0);
          iterationSpin->setSingleStep(1);
          iterationSpin->setMinimum(1);
          iterationSpin->setValue(mIteration);
          iterationSpin->setMaximum(std::numeric_limits<double>::max());
          connect(iterationSpin, &QDoubleSpinBox::valueChanged, this,
                  &MainWindow::resultCountChanged);
        }
        iterationHBox->addWidget(new QLabel("RANSAC iteration:"));
        iterationHBox->addWidget(iterationSpin);
      }
      buttons->addLayout(iterationHBox);

      auto avgHBox = new QHBoxLayout;
      {
        auto avgBox = new QCheckBox;
        {
          if (mAvgFilther) {
            avgBox->setCheckState(Qt::CheckState::Checked);
          } else {
            avgBox->setCheckState(Qt::CheckState::Unchecked);
          }
          connect(avgBox, &QCheckBox::stateChanged, this,
                  &MainWindow::avgFiltherChanged);
        }
        avgHBox->addWidget(new QLabel("avgFilther:"));
        avgHBox->addWidget(avgBox);
      }
      buttons->addLayout(avgHBox);
    }
    main->addLayout(buttons);
  }
  widget->setLayout(main);

  this->setCentralWidget(widget);
}

// -----------------------------------------------------------------------------
