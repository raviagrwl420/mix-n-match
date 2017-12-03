/**
	@file 		part_base.h
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		December 1, 2017
	@version 	1.0

	@brief		CMPT-764: Project, Header file for PartBase class.
*/

#ifndef PART_BASE_H
#define PART_BASE_H

#include <cgal.h>

enum DisplayType {FLAT_SHADED, SMOOTH_SHADED, WIREFRAME, SHADED_WITH_EDGES};

class PartBase {
public:
	Vector color;
	BoundingBox boundingBox;

	PartBase () {};
	~PartBase () {};

	virtual void render (DisplayType displayType) {};
};

#endif