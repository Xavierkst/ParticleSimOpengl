#pragma once
#include <vector>
using namespace std;
class Point {
public:
	Point(float x, float y) {
		pos.push_back(x);
		pos.push_back(y);
	}
	float getX() const { return pos[0]; }
	float getY() const { return pos[1]; }

private:
	std::vector<float> pos;
};

float calcDiam(const Point& p1, const Point& p2) {
	return sqrtf(powf(p1.getX() - p2.getX(), 2.0f) + powf(p1.getY() - p2.getY(), 2.0f));
}

float maxDiam(const vector<Point>& points) {
	float curMax = 0.0f;
	for (int i = 0; i < points.size(); ++i) {
		for (int j = i; j < points.size(); ++j) {
			curMax = max(curMax, calcDiam(points[i], points[j]));
		}
	}

	return curMax;
}
