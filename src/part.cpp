/**
	@file 		part.cpp
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		December 1 , 2017
	@version 	1.0

	@brief		CMPT-764: Assignment 2, Implements Part class.
*/

#include <part.h>

Part::Part (string label, Mesh mesh) {
	this->setLabel(label +  "_Part");

	auto faceNormals = mesh.add_property_map<FaceIndex, Vector>("f:normals", CGAL::NULL_VECTOR).first;
	auto vertexNormals = mesh.add_property_map<VertexIndex, Vector>("v:normals", CGAL::NULL_VECTOR).first;

	CGAL::Polygon_mesh_processing::compute_normals(mesh,
		vertexNormals,
		faceNormals,
		CGAL::Polygon_mesh_processing::parameters::vertex_point_map(mesh.points()).
		geom_traits(K()));


	boundingBox = CGAL::Polygon_mesh_processing::bbox(mesh,
		CGAL::Polygon_mesh_processing::parameters::vertex_point_map(mesh.points()).
		geom_traits(K()));

	this->fitLine = getLeastSquareFitLine(mesh);
	this->fitPlane = getLeastSquareFitPlane(mesh);
	this->fitSegment = getLeastSquareFitSegment(mesh);

	bool curved = false;
	if (label != string("LegFrontLeft") && label != string("LegFrontRight") && label != string("RightBar") && label != string("LeftBar") && label != string("chairfancy4") && label != string("chair2721")) {
		try {
			this->skeleton = getSkeleton(mesh);
		} catch (...) {
			std::cout << "Exception With Skeleton!!" << std::endl; 
		}

		curved = isSkeletonCurved(skeleton);
	}

	bool planar = isPlanar(mesh);
	if (curved) {
		this->primitive = SKELETON;
	} else if (planar) {
		this->primitive = PLANE;
	} else {
		this->primitive = LINE;
	}
	
	this->mesh = mesh;
}

Part* Part::initPart (string label, string filename) {
	map<int, VertexIndex> vertexMap;

	ifstream smf_file;
	smf_file.open(filename.c_str());

	if (smf_file.fail()) {
		cerr << "Error opening file." << endl << "Exiting..." << endl;
		exit(1);
	}

	// Read Data
	Mesh m;
	char type;
	int count = 1;

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
			default:
				getline(smf_file, line); // Ignore arbitrary lines
				break;
		}
	}

	smf_file.close();

	return new Part(label, m);
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

int Part::writeToFile (ofstream& smf_file, int vertexStartIndex, Transformation t) {
	map<VertexIndex, int> vertexIndexMap;

	int count = vertexStartIndex;
	for (VertexIndex v: mesh.vertices()) {
		Point p = mesh.point(v);
		Transformation total_transform = transformation * t;
		p = total_transform.transform(p);
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

	return count;
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

void renderSkeleton (Skeleton skeleton) {
	glBegin(GL_LINES);

	BOOST_FOREACH(Skeleton_edge e, edges(skeleton)) {
		const Point& s = skeleton[source(e, skeleton)].point;
		const Point& t = skeleton[target(e, skeleton)].point;
		glVertex3f(s.x(), s.y(), s.z());
		glVertex3f(t.x(), t.y(), t.z());
	}

	glEnd();
}

void renderSegment (Segment segment) {
	glBegin(GL_LINES);
		Point p1 = segment.source();
		Point p2 = segment.target();

		glVertex3f(p1.x(), p1.y(), p1.z());
		glVertex3f(p2.x(), p2.y(), p2.z());
	glEnd();
}

void Part::renderPrimitive () {
	if (primitive == SKELETON)
		renderSkeleton(skeleton);
	if (primitive == LINE)
		renderSegment(fitSegment);
	if (primitive == PLANE)
		renderBoundingBox();
}

void Part::render (DisplayType displayType) {
	glPushMatrix();
	glMultMatrixf(getTransformationArray());
	
	glColor3f(this->color[0], this->color[1], this->color[2]);

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
		case PRIMITIVES:
			renderPrimitive();
			break;
	}

	glPopMatrix();
}

void Part::renderForProjection (double scale, Vector center) {
	glPushMatrix();
	glMultMatrixf(getTransformationArray());

	glBegin(GL_TRIANGLES);

		for (FaceIndex f: faces(mesh)) {
			for (VertexIndex v: vertices_around_face(mesh.halfedge(f), mesh)) {
				Vector point(mesh.point(v).x(), mesh.point(v).y(), mesh.point(v).z());
				point -= center;
				point *= 1.2f / scale;
				glVertex3f(point.x(), point.y(), point.z());
			}
		}

	glEnd();

	glPopMatrix();
}

PartBase* Part::make_copy() {

	string theLabel = this->label;
	int s_idx = this->label.find("_Part");
	theLabel = theLabel.erase(s_idx, 5);

	Mesh kidMesh(this->mesh);

	// Apply Transformation
	for (VertexIndex v: kidMesh.vertices()) {
		mesh.point(v) = (this->transformation).transform(mesh.point(v));
	}

	Part* newPart = new Part(theLabel, kidMesh);	
	return newPart;
}

void Part::transformTo (Part *part2) {
	if (this->primitive == part2->primitive) {
		switch (this->primitive) {
			case LINE:
				this->applyTransformation(getTransformation(part2->fitSegment, this->fitSegment));
				break;
			case PLANE:
				this->applyTransformation(getTransformation(part2->boundingBox, this->boundingBox));
				break;
		}
	}
}