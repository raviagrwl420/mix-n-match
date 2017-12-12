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

double getMinimumDistance (Mesh mesh1, Mesh mesh2) {
	Polytope_distance pd(mesh1.points().begin(), mesh1.points().end(), mesh2.points().begin(), mesh2.points().end());

	return sqrt(CGAL::to_double (pd.squared_distance_numerator()) / CGAL::to_double (pd.squared_distance_denominator()));
}

Skeleton getSkeleton (Mesh mesh) {
	Skeleton skeleton;
	CGAL::extract_mean_curvature_flow_skeleton(mesh, skeleton);
	return skeleton;
}

Vector normalize (Vector vec) {
	return vec / sqrt(vec.squared_length());
}

bool isSkeletonCurved (Skeleton skeleton) {
	PolyLines polylines(skeleton);
	CGAL::split_graph_into_polylines(skeleton, polylines);

	if (polylines.polyline_count != 1) 
		return false;

	std::vector<Point> points = polylines.points;

	Point start = points[0];
	Point end = points[points.size() - 1];

	Vector direction = normalize(end - start);

	for (int i = 0; i < points.size()-1; i++) {
		Point p1 = points[i];
		Point p2 = points[i+1];

		Vector localDirection = normalize(p2 - p1);

		if (direction * localDirection < SKELETON_CURVED_THRESHOLD) {
			return true;
		}
	}

	return false;
}

Line getLeastSquareFitLine (Mesh mesh) {
	Line line;
	linear_least_squares_fitting_3(mesh.points().begin(),mesh.points().end(),line, CGAL::Dimension_tag<0>());
	return line;
}

Plane getLeastSquareFitPlane (Mesh mesh) {
	Plane plane;
	linear_least_squares_fitting_3(mesh.points().begin(),mesh.points().end(),plane,CGAL::Dimension_tag<0>());
	return plane;	
}

Segment getSegmentProjectionOnALine (Mesh mesh, Line l) {
	Vector vec = l.to_vector();

	Origin o;
	Point centroid = CGAL::centroid(mesh.points().begin(), mesh.points().end(), CGAL::Dimension_tag<0>());

	Point lowest = centroid;
	Point highest = centroid;

	Vector lowestVec = centroid - o;
	Vector highestVec = centroid - o;

	for (VertexIndex v: mesh.vertices()) {
		Point p = mesh.point(v);
		Vector newVec = p - o;

		if (vec * newVec < vec * lowestVec) {
			lowestVec = newVec;
			lowest = p;
		}

		if (vec * newVec > vec * highestVec) {
			highestVec = newVec;
			highest = p;
		}
	}

	return Segment(l.projection(lowest), l.projection(highest));
}

Segment getLeastSquareFitSegment (Mesh mesh) {
	Line l = getLeastSquareFitLine(mesh);
	
	return getSegmentProjectionOnALine(mesh, l);
}

Point getMidPoint (Segment segment) {
	Point s = segment.source();
	Point t = segment.target();

	return s + ((t - s) / 2);
}

Segment getSegmentPerpendicularToLeastSquareFit (Mesh mesh) {
	Segment s1 = getLeastSquareFitSegment(mesh);
	Point mid = getMidPoint(s1);

	Plane plane = getLeastSquareFitPlane(mesh);
	
	Line p = plane.perpendicular_line(mid);
	Vector vecP = p.to_vector();

	Line l = getLeastSquareFitLine(mesh);
	Vector vecL = l.to_vector();

	Vector cross = CGAL::cross_product(vecL, vecP);
	Line perpendicular = Line(mid, cross);

	return getSegmentProjectionOnALine(mesh, perpendicular);
};

bool isPlanar (Mesh mesh) {
	Segment s1 = getLeastSquareFitSegment(mesh);
	Segment s2 = getSegmentPerpendicularToLeastSquareFit(mesh);
	
	double l1 = sqrt(s1.squared_length());
	double l2 = sqrt(s2.squared_length());
	double ratio = l1/l2;

	return PLANAR_LOW_THRESHOLD < ratio && PLANAR_HIGH_THRESHOLD > ratio;
}

bool isCircular (Mesh mesh) {
	if (!isPlanar(mesh)) {
		return false;
	}

	Plane plane = getLeastSquareFitPlane(mesh);
	std::vector<Point> projections;

	for (VertexIndex v: mesh.vertices()) {
		Point p = mesh.point(v);
		projections.push_back(plane.projection(p));
	}

	Point centroid = CGAL::centroid(projections.begin(), projections.end(), CGAL::Dimension_tag<0>());

	Segment segment1 = getLeastSquareFitSegment(mesh);
	double radius1 = sqrt(segment1.squared_length()) / 2;

	Segment segment2 = getSegmentPerpendicularToLeastSquareFit(mesh);
	double radius2 = sqrt(segment2.squared_length()) / 2;

	double radius = (radius1 + radius2) / 2;

	for (Point p: projections) {
		Segment newSegment = Segment(centroid, p);
		double distance = sqrt(newSegment.squared_length());
		if (distance > radius + CIRCULAR_THRESHOLD)
			return false;
	}

	return true;
}