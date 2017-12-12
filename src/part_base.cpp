/**
	@file 		part_base.cpp
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		December 1 , 2017
	@version 	1.0

	@brief		CMPT-764: Assignment 2, Implements PartBase class.
*/

#include <part_base.h>

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

float *PartBase::getTransformationArray () {
	float *transformationArray = new float[16];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int index = 4*j + i;
			transformationArray[index] = transformation.m(i, j);
		}
	}

	return transformationArray;
}

void PartBase::translate (Vector vec) {
	Transformation translation(CGAL::TRANSLATION, vec);
	transformation = translation*transformation;
}