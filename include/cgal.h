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

#include <iostream>
#include <math.h>

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef K::Vector_3 Vector;
typedef CGAL::Surface_mesh<Point> Mesh;
typedef Mesh::Vertex_index VertexIndex;
typedef Mesh::Face_index FaceIndex;
typedef Mesh::Edge_index EdgeIndex;
typedef CGAL::Bbox_3 BoundingBox;

// Optimal Distances Package
typedef CGAL::Polytope_distance_d_traits_3<K, ET, double> Traits;
typedef CGAL::Polytope_distance_d<Traits> Polytope_distance;

// Skeletonization Package
typedef CGAL::Mean_curvature_flow_skeletonization<Mesh> Skeletonization;
typedef Skeletonization::Skeleton Skeleton;
typedef Skeleton::vertex_descriptor Skeleton_vertex;
typedef Skeleton::edge_descriptor Skeleton_edge;

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

#endif
