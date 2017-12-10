/**
	@file 		part_base.h
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		December 1, 2017
	@version 	1.0

	@brief		CMPT-764: Project, Header file for PartBase class.
*/

#ifndef PART_BASE_H
#define PART_BASE_H

#include <string>

#include <GL/glut.h>

#include <cgal.h>

using std::string;

enum DisplayType {FLAT_SHADED, SMOOTH_SHADED, WIREFRAME, SHADED_WITH_EDGES, PRIMITIVES};

class PartBase {
public:
	string label;
	Vector color;
	BoundingBox boundingBox;

	PartBase () {};
	~PartBase () {};

	void setLabel (string label) {
		this->label = label;
	};

	virtual PartBase *getMember (string label) {};

	virtual void setMember (string label, PartBase *part) {};

	virtual void render (DisplayType displayType) {};

	void renderBoundingBox ();
	Vector getCenter ();
};

void PartBase::renderBoundingBox () {
	glBegin(GL_LINES);
		glVertex3f(boundingBox.xmin(), boundingBox.ymin(), boundingBox.zmin());
		glVertex3f(boundingBox.xmax(), boundingBox.ymin(), boundingBox.zmin());

		glVertex3f(boundingBox.xmax(), boundingBox.ymin(), boundingBox.zmin());
		glVertex3f(boundingBox.xmax(), boundingBox.ymax(), boundingBox.zmin());

		glVertex3f(boundingBox.xmax(), boundingBox.ymax(), boundingBox.zmin());
		glVertex3f(boundingBox.xmin(), boundingBox.ymax(), boundingBox.zmin());

		glVertex3f(boundingBox.xmin(), boundingBox.ymax(), boundingBox.zmin());
		glVertex3f(boundingBox.xmin(), boundingBox.ymin(), boundingBox.zmin());


		glVertex3f(boundingBox.xmin(), boundingBox.ymin(), boundingBox.zmin());
		glVertex3f(boundingBox.xmin(), boundingBox.ymin(), boundingBox.zmax());

		glVertex3f(boundingBox.xmax(), boundingBox.ymin(), boundingBox.zmin());
		glVertex3f(boundingBox.xmax(), boundingBox.ymin(), boundingBox.zmax());

		glVertex3f(boundingBox.xmax(), boundingBox.ymax(), boundingBox.zmin());
		glVertex3f(boundingBox.xmax(), boundingBox.ymax(), boundingBox.zmax());

		glVertex3f(boundingBox.xmin(), boundingBox.ymax(), boundingBox.zmin());
		glVertex3f(boundingBox.xmin(), boundingBox.ymax(), boundingBox.zmax());


		glVertex3f(boundingBox.xmin(), boundingBox.ymin(), boundingBox.zmax());
		glVertex3f(boundingBox.xmax(), boundingBox.ymin(), boundingBox.zmax());

		glVertex3f(boundingBox.xmax(), boundingBox.ymin(), boundingBox.zmax());
		glVertex3f(boundingBox.xmax(), boundingBox.ymax(), boundingBox.zmax());

		glVertex3f(boundingBox.xmax(), boundingBox.ymax(), boundingBox.zmax());
		glVertex3f(boundingBox.xmin(), boundingBox.ymax(), boundingBox.zmax());

		glVertex3f(boundingBox.xmin(), boundingBox.ymax(), boundingBox.zmax());
		glVertex3f(boundingBox.xmin(), boundingBox.ymin(), boundingBox.zmax());
	glEnd();
}

Vector PartBase::getCenter () {
	double x = (boundingBox.xmin() + boundingBox.xmax())/2;
	double y = (boundingBox.ymin() + boundingBox.ymax())/2;
	double z = (boundingBox.zmin() + boundingBox.zmax())/2;
	return Vector(x, y, z);
}

#endif