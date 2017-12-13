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

enum Primitive {SKELETON, LINE, PLANE};

class Part : public PartBase {
public:
	Mesh mesh;
	Skeleton skeleton;
	Line fitLine;
	Plane fitPlane;
	Segment fitSegment;
	Primitive primitive;

	Part () {};
	~Part () {};
	Part (string label, Mesh mesh);
	static Part* initPart (string label, string filename);
	void writePart (string filename);
	void render (DisplayType displayType) override;
	void renderForProjection (double scale, Vector center) override;

	void renderPrimitive ();
};

#endif