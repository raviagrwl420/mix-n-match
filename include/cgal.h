/**
	@file 		cgal.h
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		December 1, 2017
	@version 	1.0

	@brief		CMPT-764: Project, Header file for CGAL.
*/

#ifndef CGAL_H
#define CGAL_H

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <boost/foreach.hpp>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>
#include <CGAL/Polygon_mesh_processing/bbox.h>

// Optimal Distances Package
#include <CGAL/Polytope_distance_d.h>
#include <CGAL/Polytope_distance_d_traits_3.h>
#include <CGAL/Homogeneous.h>

#ifdef CGAL_USE_GMP
#include <CGAL/Gmpzf.h>
typedef CGAL::Gmpzf ET;
#else
#include <CGAL/MP_Float.h>
typedef CGAL::MP_Float ET;
#endif

// Skeletonization Package
#include <CGAL/extract_mean_curvature_flow_skeleton.h>
#include <CGAL/boost/graph/split_graph_into_polylines.h>

// PCA
#include <CGAL/linear_least_squares_fitting_3.h>
#include <CGAL/centroid.h>
#include <CGAL/Origin.h>

#include <iostream>
#include <vector>
#include <math.h>

#define SKELETON_CURVED_THRESHOLD 0.7

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef K::Line_3 Line;
typedef K::Segment_3 Segment;
typedef K::Plane_3 Plane;
typedef K::Vector_3 Vector;
typedef CGAL::Surface_mesh<Point> Mesh;
typedef Mesh::Vertex_index VertexIndex;
typedef Mesh::Face_index FaceIndex;
typedef Mesh::Edge_index EdgeIndex;
typedef CGAL::Bbox_3 BoundingBox;
typedef CGAL::Origin Origin;

// Optimal Distances Package
typedef CGAL::Polytope_distance_d_traits_3<K, ET, double> Traits;
typedef CGAL::Polytope_distance_d<Traits> Polytope_distance;

// Skeletonization Package
typedef CGAL::Mean_curvature_flow_skeletonization<Mesh> Skeletonization;
typedef Skeletonization::Skeleton Skeleton;
typedef Skeleton::vertex_descriptor Skeleton_vertex;
typedef Skeleton::edge_descriptor Skeleton_edge;

struct PolyLines {
	const Skeleton& skeleton;
	std::vector<Point> points;
	int polyline_count = 0;

	PolyLines (const Skeleton& skeleton): skeleton(skeleton) {};

	void start_new_polyline () {
		++polyline_count;
	}

	void add_node (Skeleton_vertex v) {
		points.push_back(skeleton[v].point);
	}

	void end_polyline() {

	}
};

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
};

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

	std::cout << "Size:: " << points.size() << std::endl;

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

Segment getLeastSquareFitSegment (Mesh mesh) {
	Line l = getLeastSquareFitLine(mesh);
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

#endif
