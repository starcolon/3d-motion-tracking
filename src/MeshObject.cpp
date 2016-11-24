#include "MeshObject.h"

MeshObject::MeshObject(vector<Point2f> &ps)
{
  this->points = ps;
}

vector<TrackedPoint> MeshObject::toKeyPoint2fs() const
{
  vector<TrackedPoint> kps;
  for (Point2f p: this->points)
    kps.push_back(TrackedPoint(p));
  return kps;
}

MeshObject MeshObject::align(MeshObject &newMesh)
{
  // TAOTODO:
  MeshObject out;

  
  return out;
}

Point2f MeshObject::centroid() const
{
  if (this->isEmpty())
    throw "No elements";
  double sumx = 0;
  double sumy = 0;
  double N    = (double)this->points.size();

  for (Point2f p : this->points)
  {
    sumx += p.x;
    sumy += p.y;
  }

  return Point2f(sumx/N, sumy/N);
}

double MeshObject::findMinDistance(Point2f p, vector<Point2f> ps)
{
  double minDist = numeric_limits<double>::max();
  for (Point2f q : ps)
  {
    double d = _dist(p, q);
    minDist = d < minDist ? d : minDist;
  }
  return minDist;
}

vector<MeshObject> MeshObject::split(double maxEdgeLength) const
{
  vector<MeshObject> ms;

  // Traverse through the mesh graph 
  vector<Point2f> points0(this->points.begin(), this->points.end());
  vector<Point2f> residues;

  while (!points0.empty())
  {
    vector<Point2f> neighbors;
    
    // Pick up next candidate
    Point2f p0 = points0.back();
    points0.pop_back();
    neighbors.push_back(p0);

    // Traverse through the remaining points
    while (!points0.empty())
    {
      Point2f p = points0.back();
      points0.pop_back();
      double d  = MeshObject::findMinDistance(p, neighbors);

      // If the point is close to the current aggregating mesh, 
      // count it in
      if (d<maxEdgeLength)
        neighbors.push_back(p);
      else
        residues.push_back(p);
    }

    // Done
    MeshObject m(neighbors);
    ms.push_back(m);

    points0 = residues;
  }

  return ms;
}

bool MeshObject::isEmpty() const
{
  return this->points.empty();
}


void MeshObject::drawMesh(Mat &canvas, Scalar edgeColor, Scalar vertexColor, double maxDistance) const
{
  auto N = this->points.size();
  // Draw edges
  for (int i=0; i<N; i++)
    for (int j=i+1; j<N; j++)
    {
      Point2f p0 = this->points[i];
      Point2f p  = this->points[j];
      if (_dist(p0,p) <= maxDistance)
        line(canvas, p0, p, edgeColor, 1, CV_AA);
    }
  // Draw vertices
  for (Point2f p : this->points)
    circle(canvas, p, 1, vertexColor, 1, CV_AA);
}


