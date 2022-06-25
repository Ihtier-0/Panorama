#include "MainWindow.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>

#include "BRIEF/BRIEF.h"
#include "Combine/Combine.h"
#include "FAST/FAST.h"
#include "RANSAC/RANSAC.h"
#include "blur/blur.h"

AspectRatioLabel::AspectRatioLabel(QWidget *parent) : QLabel(parent) {
  this->setMinimumSize(1, 1);
  setScaledContents(false);
}

void AspectRatioLabel::setPixmap(const QPixmap &p) {
  pix = p;
  QLabel::setPixmap(p.isNull() ? pix : scaledPixmap());
}

int AspectRatioLabel::heightForWidth(int width) const {
  return pix.isNull() ? this->height()
                      : ((qreal)pix.height() * width) / pix.width();
}

QSize AspectRatioLabel::sizeHint() const {
  int w = this->width();
  return QSize(w, heightForWidth(w));
}

QPixmap AspectRatioLabel::scaledPixmap() const {
  return pix.scaled(this->size(), Qt::KeepAspectRatio,
                    Qt::SmoothTransformation);
}

void AspectRatioLabel::resizeEvent(QResizeEvent *e) {
  if (!pix.isNull()) {
    QLabel::setPixmap(scaledPixmap());
  }
  QLabel::resizeEvent(e);
}

// -----------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
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

  if (similar.isEmpty()) {
    return similar;
  }

  qDebug() << "RANSAC...";
  timer.start();
  int count = similar.size() * (mResultCount / 100.0f);
  QVector<QPair<int, int>> bestSimilar =
      RANSAC(similar, left.second, right.second, count ? count : 1, mIteration);
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

Loss MainWindow::getLoss() const { return mLoss; }

void MainWindow::setLoss(Loss newLoss) {
  if (mLoss == newLoss)
    return;
  mLoss = newLoss;
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

void MainWindow::lossChanged(const QString &text) {
  qDebug() << "lossChanged" << text;
  if (text == "L1") {
    qDebug() << "\tL1";
    mLoss = Loss::L1;
  } else if (text == "L2") {
    qDebug() << "\tL2";
    mLoss = Loss::L2;
  } else {
    qDebug() << "\tunknown";
  }
}

// -----------------------------------------------------------------------------

void MainWindow::openLeft() {
  if (!openImage(mLeft)) {
    return;
  }
  qDebug() << "openLeft";

  mLeftImg->setPixmap(QPixmap::fromImage(mLeft));
}

void MainWindow::openRight() {
  if (!openImage(mRight)) {
    return;
  }
  qDebug() << "openRight";

  mRightImg->setPixmap(QPixmap::fromImage(mRight));
}

// -----------------------------------------------------------------------------

void MainWindow::combine() {
  if (mLeft.isNull()) {
    QMessageBox::warning(this, "Warning!", "Open left image before combine");
    return;
  }
  if (mRight.isNull()) {
    QMessageBox::warning(this, "Warning!", "Open right image before combine");
    return;
  }

  qDebug() << "combine";

  mLeft.convertTo(QImage::Format_RGB888);
  mRight.convertTo(QImage::Format_RGB888);

  mBRIEFSequence = BRIEFSequence();

  auto left = beforeCombine(mLeft, "left");
  qDebug();
  auto right = beforeCombine(mRight, "right");
  qDebug();

  const auto leftWidht = mLeft.width();
  const auto rightWidht = mRight.width();

  const auto leftHeight = mLeft.height();
  const auto rightHeight = mRight.height();

  const auto rightToBothCoord = [leftWidht](const QPoint &point) -> QPoint {
    return {point.x() + leftWidht, point.y()};
  };

  if (left.first.isEmpty() || right.first.isEmpty()) {
    QMessageBox::warning(
        this, "Not found features",
        "Not found similar features, change parameters and try again");
    return;
  }

  auto similar = findSimilar(left, right);

  if (similar.isEmpty()) {
    QMessageBox::warning(
        this, "Not found similar features",
        "Not found similar features, change parameters and try again");
    return;
  }

  if (mAvgFilther) {
    similar = avgFilter(left.second, right.second, similar);

    if (mDebug) {
      QImage bothImage(leftWidht + rightWidht, qMax(leftHeight, rightHeight),
                       QImage::Format_RGB888);
      bothImage.fill(QColorConstants::Yellow);

      QPainter painter(&bothImage);
      painter.drawImage(0, 0, mLeft);
      painter.drawImage(leftWidht, 0, mRight);

      for (const auto &s : similar) {
        painter.setPen(randomColor());
        painter.drawLine(
            QPoint(left.first[s.first].x(), left.first[s.first].y()),
            rightToBothCoord(
                QPoint(right.first[s.second].x(), right.first[s.second].y())));
      }

      bothImage.save("avg filter similar.jpg");
    }
  }

  const auto initial =
      initialApproximation(left.first, right.first, left.second, right.second,
                           similar, rightToBothCoord);

  {
    mCombine = QImage(leftWidht + rightWidht + qAbs(initial.x()),
                      qMax(leftHeight, rightHeight) + qAbs(initial.y()),
                      QImage::Format_RGB888);
    mCombine.fill(QColorConstants::Yellow);
    QPainter painter(&mCombine);
    painter.drawImage(0, qAbs(initial.y()), mLeft);
    painter.drawImage(rightToBothCoord({0, 0}).x() + initial.x(),
                      rightToBothCoord({0, 0}).y() + initial.y(), mRight);

    if (mDebug) {
      mCombine.save("initialApproximation.jpg");
    }
  }

  const auto opt =
      optimize(left.first, right.first, left.second, right.second, similar,
               rightToBothCoord, {0, initial, {1, 1}}, mLoss);
  qDebug() << initial;
  qDebug() << opt.rotate << opt.translate << opt.scale;
  {
    mCombine = QImage(leftWidht + rightWidht + qAbs(initial.x()),
                      qMax(leftHeight, rightHeight) + qAbs(initial.y()),
                      QImage::Format_RGB888);
    mCombine.fill(QColorConstants::Yellow);
    QPainter painter(&mCombine);
    painter.drawImage(0, qAbs(initial.y()), mLeft);

    QTransform transform;
    transform.scale(opt.scale.x(), opt.scale.y());
    transform.rotate(opt.rotate);
    transform.translate(opt.translate.x(), opt.translate.y());
    painter.setTransform(transform);

    painter.drawImage(rightToBothCoord({0, 0}).x(),
                      rightToBothCoord({0, 0}).y(), mRight);

    if (mDebug) {
      mCombine.save("optimize.jpg");
    }
  }

  mCombineImg->setPixmap(QPixmap::fromImage(mCombine));

  mCombineWidget->update();
  mCombineWidget->show();
}

// -----------------------------------------------------------------------------

void MainWindow::clearImages() {
  qDebug() << "clearImages";

  mLeftImg->setPixmap(QPixmap::fromImage(QImage()));
  mLeftImg->setText("left image");
  mRightImg->setPixmap(QPixmap::fromImage(QImage()));
  mRightImg->setText("right image");

  mLeft = QImage();
  mRight = QImage();
}

// -----------------------------------------------------------------------------

void MainWindow::saveResult() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save File"), "/home/jana/untitled.png",
      tr("Images (*.png *.xpm *.bmp *.jpg *.jpeg)"));

  if (fileName.isEmpty()) {
    return;
  }

  if (mCombine.isNull()) {
    qDebug() << "mCombine null";
    return;
  }

  qDebug() << "saveResult";

  mCombine.save(fileName);
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
    auto leftImageVBox = new QVBoxLayout;
    {
      auto openLeftButton = new QPushButton("open left");
      {
        connect(openLeftButton, &QPushButton::pressed, this,
                &MainWindow::openLeft);
      }
      leftImageVBox->addWidget(openLeftButton);

      mLeftImg = new AspectRatioLabel;
      mLeftImg->setText("left image");
      mLeftImg->setAlignment(Qt::AlignCenter);
      leftImageVBox->addWidget(mLeftImg);

      auto combineButton = new QPushButton("combine");
      {
        connect(combineButton, &QPushButton::pressed, this,
                &MainWindow::combine);
      }

      leftImageVBox->addWidget(combineButton);
    }
    main->addLayout(leftImageVBox, 1);

    auto rightImageVBox = new QVBoxLayout;
    {
      auto openRightButton = new QPushButton("open right");
      {
        connect(openRightButton, &QPushButton::pressed, this,
                &MainWindow::openRight);
      }
      rightImageVBox->addWidget(openRightButton);

      mRightImg = new AspectRatioLabel;
      mRightImg->setText("right image");
      mRightImg->setAlignment(Qt::AlignCenter);
      rightImageVBox->addWidget(mRightImg);

      auto clearButton = new QPushButton("clear");
      {
        connect(clearButton, &QPushButton::pressed, this,
                &MainWindow::clearImages);
      }
      rightImageVBox->addWidget(clearButton);
    }
    main->addLayout(rightImageVBox, 1);

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
          resultCountSpin->setMaximum(100);
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

      auto lossHBox = new QHBoxLayout;
      {
        auto loss = new QComboBox;
        {
          loss->addItem("L1");
          loss->addItem("L2");
          loss->setCurrentIndex((int)mLoss);
          connect(loss, &QComboBox::currentTextChanged, this,
                  &MainWindow::lossChanged);
        }
        lossHBox->addWidget(new QLabel("loss:"));
        lossHBox->addWidget(loss);
      }
      buttons->addLayout(lossHBox);
    }
    main->addLayout(buttons, 0);
  }
  widget->setLayout(main);

  this->setCentralWidget(widget);

  this->setStyleSheet("*"
                      "{"
                      "  background-color: rgb(40, 36, 44);"
                      "  color: white;"
                      "}");

  auto mainCombine = new QVBoxLayout;
  {
    mCombineImg = new AspectRatioLabel;
    mCombineImg->setAlignment(Qt::AlignCenter);
    mCombineImg->setText("combine");
    mainCombine->addWidget(mCombineImg, Qt::AlignCenter);

    auto saveButton = new QPushButton("save");
    {
      connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveResult);
    }
    mainCombine->addWidget(saveButton);
  }
  mCombineWidget->setLayout(mainCombine);

  mCombineWidget->setParent(this, Qt::Window);

  mCombineWidget->setStyleSheet("*"
                                "{"
                                "  background-color: rgb(40, 36, 44);"
                                "  color: white;"
                                "}");
  mCombineWidget->setMinimumSize(300, 300);
  mCombineWidget->hide();
}

// -----------------------------------------------------------------------------

bool MainWindow::openImage(QImage &image) {
  QString fileName = QFileDialog::getOpenFileName(
      this, tr("Open Image"), "/home",
      tr("Images (*.png *.xpm *.bmp *.jpg *.jpeg)"));

  if (fileName.isEmpty()) {
    return false;
  }

  if (!image.load(fileName)) {
    qDebug() << "can't open file";
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
