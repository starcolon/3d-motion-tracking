#include "Alignment.h"

Alignment::Alignment(function<double (Point2f, Point2f)> measureDistance, function<double (Mat, Mat)> measureFeatureSimilarity, double maxMoveDistance)
{
  cout << GREEN << "Initialising alignment engine..." << RESET << endl;
  this->measureDistFunction       = measureDistance;
  this->measureSimilarityFunction = measureFeatureSimilarity;
  this->maxDistance               = maxMoveDistance;
  this->isVisualisationOn         = false;
}

void Alignment::setVisualisation(bool on)
{
  this->isVisualisationOn = on;
}

unordered_map<int,int> Alignment::align(vector<Point2f> basepoints, vector<Point2f> newpoints, const Mat baseFeatures, const Mat newFeatures)
{
  int i = 0;
  int M = VIS_MAX_SPOT * VIS_PATCH_SIZE + 1;
  Mat matchScore = Mat::zeros(basepoints.size(), newpoints.size(), CV_64FC1);
  Mat vis = Mat::zeros(M, M, CV_8UC3);
  unordered_map<int,int> pairs;
  
  // Record the population distribution (of scores)
  GenericDistribution<double> scorePopulation;
  for (auto bp : basepoints)
  {
    // List of Tuples of <distance, index of candidate>
    // Closest first
    priority_queue<distanceToIndex, vector<distanceToIndex>, compareDistance> candidates;

    // Measure all candidates within the perimeter
    int j = 0;
    for (auto np : newpoints)
    {
      double d = this->measureDistFunction(bp, np);
      if (d > this->maxDistance)
      {
        // NOTE: Do not add zero as a candidate
        matchScore.at<double>(i,j) = 0;
      }
      else
      {
        auto v0 = baseFeatures.row(i);
        auto v1 = newFeatures.row(j);
        double mag0 = norm(v0, CV_L2);
        double mag1 = norm(v1, CV_L2);
        double similarity = 0.5*(M_PI - acos(v0.dot(v1)/(mag0*mag1)))/M_PI;

        double score = (d<1e-30) ? 1.0 : similarity / d;
        // TAOTODO: Reject low score statistically
        if (score > 1e-10)
          candidates.push(make_tuple(j, score));
        matchScore.at<double>(i,j) = score;

        // Record the score population
        scorePopulation.addPopulation(score);
        
        if (this->isVisualisationOn)
        {
          int v   = floor(255 * score);
          auto p0 = Point2f(i*VIS_PATCH_SIZE, j*VIS_PATCH_SIZE);
          auto p1 = Point2f((i+1)*VIS_PATCH_SIZE-1, (j+1)*VIS_PATCH_SIZE-1);
          auto c  = Scalar(0,0,int(v));
          rectangle(vis, p0, p1, c, -1);
        }
      }
      j++;
    }

    if (!candidates.empty())
    {
      auto matchedPoint = candidates.top();
      if (get<0>(matchedPoint) > 0)
        pairs.insert(make_pair(i, get<0>(matchedPoint)));
    }

    i++;
  }
  
  if (this->isVisualisationOn)
  {
    imshow("matching score", vis);
    auto binstep = Bucket<double>(0.02, 0.0, 1.0);
    auto bounds  = make_tuple(0.0, 0.1);
    #ifdef DEBUG_ALIGNMENT
    scorePopulation.bucketPlot(binstep, bounds, "Score distribution", 10);
    #endif
  }

  return pairs;
}