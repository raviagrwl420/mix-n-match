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

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef K::Vector_3 Vector;
typedef CGAL::Surface_mesh<Point> Mesh;
typedef Mesh::Vertex_index VertexIndex;
typedef Mesh::Face_index FaceIndex;
typedef Mesh::Edge_index EdgeIndex;
typedef CGAL::Bbox_3 BoundingBox;

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

#endif
