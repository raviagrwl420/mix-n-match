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

Point getCenterPoint (BoundingBox boundingBox) {
	Vector center = getCenter(boundingBox);
	return Point(center.x(), center.y(), center.z());
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

Segment orientSegment (Segment s) {
	if (s.target().z() > s.source().z()) {
		return s;
	} else {
		return Segment(s.target(), s.source());
	}
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

float getHausdorffDistance (Mesh m1, Mesh m2) {
	return CGAL::Polygon_mesh_processing::approximate_Hausdorff_distance<TAG>(m1, m2, 
		CGAL::Polygon_mesh_processing::parameters::vertex_point_map(m1.points()),
		CGAL::Polygon_mesh_processing::parameters::vertex_point_map(m2.points()));
};

Transformation getTransformation (Segment s1, Segment s2) {
	Origin o;
	Point m1 = getMidPoint(s1);
	Point m2 = getMidPoint(s2);

	// Translate to mid point
	Vector translateToOrigin = o - m2;
	Transformation translation1(CGAL::TRANSLATION, translateToOrigin);

	// Scale
	Transformation scale(CGAL::SCALING, sqrt(s1.squared_length()/s2.squared_length()));

	// Rotation
	Vector v1 = s1.to_vector();
	Vector v2 = s2.to_vector();
	Transformation rotation = getRotationMatrix(v1, v2);

	// Translation
	Vector translateToMidPoint1 = m1 - o; 
	Transformation translation2(CGAL::TRANSLATION, translateToMidPoint1);

	return translation2*rotation*scale*translation1;
};

Transformation getTransformation (BoundingBox box1, BoundingBox box2) {
	Origin o;
	Point m1 = getCenterPoint(box1);
	Point m2 = getCenterPoint(box2);

	// Translate to mid point
	Vector translateToOrigin = o - m2;
	Transformation translation1(CGAL::TRANSLATION, translateToOrigin);

	// Scale
	float s1 = getScale(box1);
	float s2 = getScale(box2);
	Transformation scale(CGAL::SCALING, sqrt(s1/s2));

	// Translation
	Vector translateToMidPoint1 = m1 - o; 
	Transformation translation2(CGAL::TRANSLATION, translateToMidPoint1);

	return translation2*scale*translation1;
};

Transformation getNonUniformTransformation (BoundingBox box1, BoundingBox box2) {
	Origin o;
	Point m1 = getCenterPoint(box1);
	Point m2 = getCenterPoint(box2);

	// Translate to mid point
	Vector translateToOrigin = o - m2;
	Transformation translation1(CGAL::TRANSLATION, translateToOrigin);

	// Scale
	float s1 = getScale(box1);
	float s2 = getScale(box2);
	float s = sqrt(s1/s2);
	Transformation scale1(CGAL::SCALING, s);

	double zRange1 = box1.zmax() - box1.zmin();
	double zRange2 = box2.zmax() - box2.zmin();
	Transformation scale2(
		1              , 0               , 0, 
		0              , 1               , 0,
		0              , 0               , zRange1/(s*zRange2), 1);

	Transformation scale = scale2*scale1;

	// Translation
	Vector translateToMidPoint1 = m1 - o; 
	Transformation translation2(CGAL::TRANSLATION, translateToMidPoint1);

	return translation2*scale*translation1;
};


Transformation getRotationMatrix (Vector v1, Vector v2) {
	Eigen::Vector3f A(v1.x(), v1.y(), v1.z());
	Eigen::Vector3f B(v2.x(), v2.y(), v2.z());

	Eigen::Matrix3f R = Eigen::Quaternionf().setFromTwoVectors(A,B).toRotationMatrix();

	return Transformation(R(0,0), R(1,0), R(2,0), R(0,1), R(1,1), R(2,1), R(0,2), R(1,2), R(2,2), 1);
};