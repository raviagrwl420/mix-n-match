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
#include <vector>

#include <GL/glut.h>

#include <cgal.h>

using std::string;

enum DisplayType {FLAT_SHADED, SMOOTH_SHADED, WIREFRAME, SHADED_WITH_EDGES, PRIMITIVES};


float const color_deeppink[3] = {1.000f, 0.078f, 0.576f};
float const color_yellow[3] = {1.000f, 1.000f, 0.000f};
float const color_blueviolet[3] = {0.541f, 0.169f, 0.886f};
float const color_slateblue[3] = {0.416f, 0.353f, 0.804f};
float const color_mediumspringgreen[3] = {0.000f, 0.980f, 0.604f};
float const color_lightcoral[3] = {0.941f, 0.502f, 0.502f};
float const color_green[3] = {0.000f, 0.502f, 0.000f};
float const color_cyan[3] = {0.000f, 1.000f, 1.000f};
float const color_darkmagenta[3] = {0.545f, 0.000f, 0.545f};
float const color_steelblue[3] = {0.275f, 0.510f, 0.706f};
float const color_blue[3] = {0.000f, 0.000f, 1.000f};
float const color_sandybrown[3] = {0.957f, 0.643f, 0.376f};
float const color_chocolate[3] = {0.824f, 0.412f, 0.118f};
float const color_saddlebrown[3] = {0.545f, 0.271f, 0.075f};
float const color_darkslategray[3] = {0.184f, 0.310f, 0.310f};
float const color_olive[3] = {0.502f, 0.502f, 0.000f};

static std::vector<std::vector<float>> color_list {	
		{color_deeppink[0], color_deeppink[1], color_deeppink[2]}, 
		{color_yellow[0], color_yellow[1], color_yellow[2]}, 
		{color_blueviolet[0], color_blueviolet[1], color_blueviolet[2]}, 
		{color_slateblue[0], color_slateblue[1], color_slateblue[2]}, 
		{color_mediumspringgreen[0], color_mediumspringgreen[1], color_mediumspringgreen[2]}, 
		{color_lightcoral[0], color_lightcoral[1], color_lightcoral[2]},
	   	{color_green[0], color_green[1], color_green[2]}, 
	   	{color_cyan[0], color_cyan[1], color_cyan[2]}, 
	   	{color_darkmagenta[0], color_darkmagenta[1], color_darkmagenta[2]}, 
	   	{color_steelblue[0], color_steelblue[1], color_steelblue[2]}, 
	   	{color_blue[0], color_blue[1], color_blue[2]}, 
	   	{color_sandybrown[0], color_sandybrown[1], color_sandybrown[2]}, 
	   	{color_chocolate[0], color_chocolate[1], color_chocolate[2]}, 
	   	{color_saddlebrown[0], color_saddlebrown[1], color_saddlebrown[2]}, 
	   	{color_darkslategray[0], color_darkslategray[1], color_darkslategray[2]}, 
	   	{color_olive[0], color_olive[1], color_olive[2]}
};

class PartBase {
public:

	

	// float color_list[16][3] = [color_deeppink, color_yellow, color_blueviolet, color_slateblue, color_mediumspringgreen, color_lightcoral,
	// 						   color_green, color_cyan, color_darkmagenta, color_steelblue, color_blue, color_sandybrown, color_chocolate, 
	// 						   color_saddlebrown, color_darkslategray, color_olive];

	string label;
	Vector color;
	BoundingBox boundingBox;
	Transformation transformation;

	PartBase () {};
	~PartBase () {};

	void setLabel (string label) {
		this->label = label;
	};

	Vector getRandomColor()
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		return Vector(r, g, b);
	}

	virtual void setColor (int color_index) {
		if(color_index < color_list.size())
			this->color = Vector(
					color_list[color_index][0],
					color_list[color_index][1],
					color_list[color_index][2]
				);
		else
		{
			Vector c = getRandomColor();
			color_list.push_back({c[0], c[1], c[2]});
			this->color = Vector(
					color_list[color_index][0],
					color_list[color_index][1],
					color_list[color_index][2]
				);
			
		}
	};	

	virtual void setColor (Vector c) {
		this->color = Vector(c[0], c[1], c[2]);		
	};	

	virtual PartBase *getMember (string label) {};

	virtual void setMember (string label, PartBase *part) {};

	virtual void render (DisplayType displayType) {};

	virtual void renderForProjection (double scale, Vector center) {};

	void renderBoundingBox ();

	Vector getCenter ();

	virtual PartBase *getMemberGlobally (string label) {};
	virtual PartBase *getParentGlobally (string label) {};
	virtual PartBase* make_copy() {};
	virtual bool removeMember(string label1) {};
	// i.e. replace the object with "label1" from part1 with the object with "label2" from part2
	virtual bool replace(string label1, PartBase *part2, string label2) {};
	virtual bool swap(string label1, PartBase *part2, string label2) {};

	// Transformations
	float *getTransformationArray ();

	void applyTransformation (Transformation transform);

	void translate (Vector vec);

	void scale (float s);
};

#endif