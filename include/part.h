/**
	@file 		part.h
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		December 1, 2017
	@version 	1.0

	@brief		CMPT-764: Project, Header file for Part class.
*/

#ifndef PART_H
#define PART_H

#include <part_base.h>
#include <cgal.h>

#include <GL/glut.h>

#include <fstream>
#include <cstdlib>
#include <string>
#include <map>
#include <iostream>

using std::map;
using std::string;
using std::ifstream;
using std::ofstream;
using std::cerr;
using std::endl;

enum DisplayType {FLAT_SHADED, SMOOTH_SHADED, WIREFRAME, SHADED_WITH_EDGES};

class Part : public PartBase {
public:
	Mesh mesh;

	Part () {};
	~Part () {};
	Part (Mesh mesh);
	static Part initPart (string filename);
	void writePart (string filename);
	void render (DisplayType displayType);
};

Part::Part (Mesh mesh) {
	auto faceNormals = mesh.add_property_map<FaceIndex, Vector>("f:normals", CGAL::NULL_VECTOR).first;
	auto vertexNormals = mesh.add_property_map<VertexIndex, Vector>("v:normals", CGAL::NULL_VECTOR).first;

	CGAL::Polygon_mesh_processing::compute_normals(mesh,
		vertexNormals,
		faceNormals,
		CGAL::Polygon_mesh_processing::parameters::vertex_point_map(mesh.points()).
		geom_traits(K()));

	this->mesh = mesh;
}

Part Part::initPart (string filename) {
	map<int, VertexIndex> vertexMap;

	ifstream smf_file;
	smf_file.open(filename.c_str());

	if (smf_file.fail()) {
		cerr << "Error opening file." << endl << "Exiting..." << endl;
		exit(1);
	}

	char type;
	int numVertices, numFaces;

	smf_file >> type >> numVertices >> numFaces; // Read first line for number of vertices and faces

	Mesh m;

	int count = 1;
	// Read remaining data
	while (smf_file >> type) {
		string line;
		float x, y, z;
		int v1, v2, v3;
		VertexIndex v;

		switch (type) {
			case '#':
				getline(smf_file, line); // Ignore comments starting with '#'
				break;
			case 'v':
				smf_file >> x >> y >> z;
				// mesh->insertVertex(x, y, z);
				v = m.add_vertex(Point(x,y,z));
				vertexMap[count] = v;
				count++;
				break;
			case 'f':
				smf_file >> v1 >> v2 >> v3;
				// mesh->insertTriangle(v1, v2, v3);
				m.add_face(vertexMap[v1], vertexMap[v2], vertexMap[v3]);
				break;
		}
	}

	smf_file.close();

	return Part(m);
};

void Part::writePart (string filename) {
	map<VertexIndex, int> vertexIndexMap;

	ofstream smf_file;
	smf_file.open(filename.c_str());

	if (smf_file.fail()) {
		cerr << "Error opening file." << endl << "Exiting..." << endl;
		exit(1);
	}

	smf_file << "# " << mesh.number_of_vertices() << " " << mesh.number_of_faces() << endl;

	int count = 1;
	for (VertexIndex v: mesh.vertices()) {
		Point p = mesh.point(v);
		smf_file << "v " << p.x() << " " << p.y() << " " << p.z() << endl;
		vertexIndexMap[v] = count;
		count++;
	}

	for (FaceIndex f: mesh.faces()) {
		smf_file << "f";
		for (VertexIndex v: vertices_around_face(mesh.halfedge(f), mesh)) {
			smf_file << " " << vertexIndexMap[v];
		}
		smf_file << endl;
	}

	smf_file.close();
}

void renderFlatShaded(Mesh mesh) {
	glShadeModel(GL_FLAT);
	glEnable(GL_NORMALIZE);
	glBegin(GL_TRIANGLES);

	auto faceNormals = mesh.property_map<FaceIndex, Vector>("f:normals").first;
	for (FaceIndex f: faces(mesh)) {
		Vector faceNormal = faceNormals[f];
		glNormal3f(faceNormal.x(), faceNormal.y(), faceNormal.z());
		for (VertexIndex v: vertices_around_face(mesh.halfedge(f), mesh)) {
			glVertex3f(mesh.point(v).x(), mesh.point(v).y(), mesh.point(v).z());
		}
	}

	glEnd();
}

void renderSmoothShaded(Mesh mesh) {
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glBegin(GL_TRIANGLES);

	auto vertexNormals = mesh.property_map<VertexIndex, Vector>("v:normals").first;
	for (FaceIndex f: faces(mesh)) {
		for (VertexIndex v: vertices_around_face(mesh.halfedge(f), mesh)) {
			Vector vertexNormal = vertexNormals[v];
			glNormal3f(vertexNormal.x(), vertexNormal.y(), vertexNormal.z());
			glVertex3f(mesh.point(v).x(), mesh.point(v).y(), mesh.point(v).z());
		}
	}

	glEnd();
}

void renderWireframe(Mesh mesh) {
	glBegin(GL_LINES);

	for (EdgeIndex e: mesh.edges()) {
		VertexIndex start = mesh.vertex(e, 0);
		VertexIndex end = mesh.vertex(e, 1);

		glVertex3f(mesh.point(start).x(), mesh.point(start).y(), mesh.point(start).z());
		glVertex3f(mesh.point(end).x(), mesh.point(end).y(), mesh.point(end).z());
	}

	glEnd();
}

void Part::render (DisplayType displayType) {
	switch (displayType) {
		case FLAT_SHADED:
			renderFlatShaded(mesh);
			break;
		case SMOOTH_SHADED:
			renderSmoothShaded(mesh);
			break;
		case WIREFRAME:
			renderWireframe(mesh);
			break;
		case SHADED_WITH_EDGES:
			renderFlatShaded(mesh);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			renderWireframe(mesh);
			break;
	}
}

#endif