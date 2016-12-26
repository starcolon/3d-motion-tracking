#ifndef HUNGARIAN
#define HUNGARIAN

#include <vector>
#include <tuple>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include <opencv/cvaux.hpp>

using namespace std;
using namespace cv;

/**
 * Hungarian algorithm implementation 
 * Compatible with [Mat<float>]
 * TAOTOREVIEW: Use template class
 */
class Hungarian
{
public:
  Hungarian(const Mat& cost, bool debug);

  // <row, col>
  vector<tuple<int, int>> optimiseMinima() const;

  // Find minimum cover lines to cover all zeroes in matrix {m}
  static tuple<set<int>, set<int>> coverZeros(Mat& m, bool debug);

  // Create augmented zeroes in the cost matrix {m}
  static void createAdditionalZeros(Mat& m, tuple<set<int>, set<int>> zeroes, bool debug);

private:
  Mat costM;
  bool debug;
  const int MAX_ITER = 3;

  // Find minimum of the i-th row
  float minOfRow(int i) const;
  // Find minimum of the i-th col
  float minOfCol(int i) const;
};


#endif