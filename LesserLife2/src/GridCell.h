#include "Point.h"
#include <algorithm>

class GridCell {
public:
	int x, y;
	std::vector<Point*> points; // List of points currently in this cell

	GridCell() : x(0), y(0) {}

	GridCell(int x, int y) : x(x), y(y) {}

    void addPoint(Point& point) {
        points.push_back(&point);
    }

    void removePoint(Point& point) {
        points.erase(std::remove(points.begin(), points.end(), &point), points.end());
    }
};
