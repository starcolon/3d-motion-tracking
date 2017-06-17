#ifndef ALIGNMENT
#define ALIGNMENT

#include <vector>
#include <queue>
#include <tuple>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "master.h"
#include "Distribution.h"

using namespace std;
using namespace cv;
using namespace Distribution;

const int VIS_PATCH_SIZE = 8;
const int VIS_MAX_SPOT   = 48;

typedef tuple<double, int> distanceToIndex;

class compareDistance
{
public:
  // Ascending order
  inline bool operator()(distanceToIndex &a, distanceToIndex &b)
  { 
    return get<0>(a) < get<0>(b); 
  }
};

class Alignment
{
protected:
  bool isVisualisationOn;
  function<double (Point2f, Point2f)> measureDistFunction;
  function<double (Mat, Mat)> measureSimilarityFunction;
  double maxDistance;
public:
  Alignment(function<double (Point2f, Point2f)> measureDistance, function<double (Mat, Mat)> measureFeatureSimilarity, double maxMoveDistance);
  void setVisualisation(bool on);
  unordered_map<int,int> align(vector<Point2f> basepoints, vector<Point2f> newpoints, const Mat baseFeatures, const Mat newFeatures);
};

#endif