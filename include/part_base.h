/**
	@file 		part_base.h
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		December 1, 2017
	@version 	1.0

	@brief		CMPT-764: Project, Header file for PartBase class.
*/

#ifndef PART_BASE_H
#define PART_BASE_H

#include <fstream>
#include <string>

#include <GL/glut.h>

#include <cgal.h>

using std::ofstream;
using std::string;

enum DisplayType {FLAT_SHADED, SMOOTH_SHADED, WIREFRAME, SHADED_WITH_EDGES, PRIMITIVES};

class PartBase {
public:
	string label;
	Vector color;
	BoundingBox boundingBox;
	Transformation transformation;

	PartBase () {};
	~PartBase () {};

	void setLabel (string label) {
		this->label = label;
	};

	virtual PartBase *getMember (string label) {};

	virtual void setMember (string label, PartBase *part) {};

	virtual int writeToFile (ofstream& smf_file, int vertexStartIndex, Transformation t) {};

	virtual void render (DisplayType displayType) {};

	virtual void renderForProjection (double scale, Vector center) {};

	void renderBoundingBox ();

	Vector getCenter ();

	// Transformations
	float *getTransformationArray ();

	void applyTransformation (Transformation transform);

	void translate (Vector vec);

	void scale (float s);
};

#endif