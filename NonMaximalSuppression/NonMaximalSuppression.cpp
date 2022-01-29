#include "NonMaximalSuppression.h"

Matrix<qreal> NMS(const Matrix<qreal> &probability,
                  const Matrix<QPoint> &directions) {
  const auto width = probability.size();
  const auto height = probability[0].size();

  const auto onImage = [width, height](const int &x, const int &y) -> bool {
    return x >= 0 && y >= 0 && x < width && y < height;
  };

  Matrix<qreal> result(width, Row<qreal>(height, 0));
  Matrix<bool> used(width, Row<bool>(height, false));

  QPoint direction;
  QVector<QPair<QPoint, qreal>> border;

  int xStep;
  int yStep;
  int length;

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (probability[x][y] > 0 && !used[x][y]) {
        border.clear();

        used[x][y] = true;
        direction = directions[x][y];

        border.push_back({{x, y}, probability[x][y]});

        length = 1;
        while (onImage(xStep = x + direction.x() * length,
                       yStep = y + direction.y() * length) &&
               directions[xStep][yStep] == direction) {
          border.push_back({{xStep, yStep}, probability[x][y]});
          used[xStep][yStep] = true;

          ++length;
        }

        length = -1;
        while (onImage(xStep = x + direction.x() * length,
                       yStep = y + direction.y() * length) &&
               directions[xStep][yStep] == direction) {
          border.push_back({{xStep, yStep}, probability[x][y]});
          used[xStep][yStep] = true;

          --length;
        }

        auto max = *std::max_element(border.begin(), border.end(),
                                     [](const QPair<QPoint, qreal> &left,
                                        const QPair<QPoint, qreal> &right) {
                                       return left.second < right.second;
                                     });

        // просто чтобы нормально сохранить картинку
        result[max.first.x()][max.first.y()] = 255;
      }
    }
  }

  return result;
}
