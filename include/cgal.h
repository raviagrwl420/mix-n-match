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

// Transformations
#include <CGAL/Aff_transformation_3.h>

// Hausdorff Distance
#include <CGAL/Polygon_mesh_processing/distance.h>

#if defined(CGAL_LINKED_WITH_TBB)
#define TAG CGAL::Parallel_tag
#else
#define TAG CGAL::Sequential_tag
#endif

// Transformation
#include <Eigen/Geometry>

#include <iostream>
#include <vector>
#include <math.h>

#define SKELETON_CURVED_THRESHOLD 0.75
#define PLANAR_LOW_THRESHOLD 0.4
#define PLANAR_HIGH_THRESHOLD 2.5
#define CIRCULAR_THRESHOLD 0.05
#define END_TO_END_CONNECTIVITY_THRESHOLD 0.05
#define END_TO_LINE_CONNECTIVITY_THRESHOLD 0.05
#define END_TO_PLANE_CONNECTIVITY_THRESHOLD 0.05
#define LINE_TO_PLANE_CONNECTIVITY_THRESHOLD 0.05 

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

// Transformation
typedef CGAL::Aff_transformation_3<K> Transformation;

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

double getScale (BoundingBox boundingBox);

Vector getCenter (BoundingBox boundingBox);

double getMinimumDistance (Mesh mesh1, Mesh mesh2);

Skeleton getSkeleton (Mesh mesh);

Vector normalize (Vector vec);

bool isSkeletonCurved (Skeleton skeleton);

Line getLeastSquareFitLine (Mesh mesh);

Plane getLeastSquareFitPlane (Mesh mesh);

Segment getSegmentProjectionOnALine (Mesh mesh, Line l);

Segment getLeastSquareFitSegment (Mesh mesh);

Point getMidPoint (Segment segment);

Segment getSegmentPerpendicularToLeastSquareFit (Mesh mesh);

bool isPlanar (Mesh mesh);

bool isCircular (Mesh mesh);

float getHausdorffDistance (Mesh m1, Mesh m2);

float isEndToEndConnected (Segment s1, Segment s2);

Transformation getTransformation (Segment s1, Segment s2);

Transformation getTransformation (BoundingBox box1, BoundingBox box2);

Transformation getRotationMatrix (Vector v1, Vector v2);

#endif
