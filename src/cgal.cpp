#include <cgal.h>

double getScale (BoundingBox boundingBox) {
	double xRange = boundingBox.xmax() - boundingBox.xmin();
	double yRange = boundingBox.ymax() - boundingBox.ymin();
	double zRange = boundingBox.zmax() - boundingBox.zmin();

	double scale = xRange > yRange ? xRange : yRange;
	scale = zRange > scale ? zRange : scale;

	return scale;
}

Vector getCenter (BoundingBox boundingBox) {
	double x = (boundingBox.xmin() + boundingBox.xmax())/2;
	double y = (boundingBox.ymin() + boundingBox.ymax())/2;
	double z = (boundingBox.zmin() + boundingBox.zmax())/2;
	return Vector(x, y, z);
}