/**
	@file 		cgal.h
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		December 1, 2017
	@version 	1.0

	@brief		CMPT-764: Project, Header file for CGAL.
*/

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <boost/foreach.hpp>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>

typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point;
typedef K::Vector_3 Vector;
typedef CGAL::Surface_mesh<Point> Mesh;
typedef Mesh::Vertex_index VertexIndex;
typedef Mesh::Face_index FaceIndex;
typedef Mesh::Edge_index EdgeIndex;
