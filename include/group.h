/**
	@file 		group.h
	@author 	Ravi Agrawal (ravia@sfu.ca)
	@date		December 1, 2017
	@version 	1.0

	@brief		CMPT-764: Project, Header file for Group class.
*/

#ifndef GROUP_H
#define GROUP_H

#include <part_base.h>

#include <vector>
#include <map>

using std::vector;
using std::map;
using std::make_pair;

class Group : public PartBase {
public:
	vector<PartBase*> members;
	map<string, int> labelIndexMap;

	Group (string label);
	~Group () {};

	void addMember (PartBase *member);
	void render (DisplayType displayType) override;
	void renderForProjection (double scale, Vector center) override;

	PartBase *getMember (string label);
	void setMember (string label, PartBase *member);
};

Group::Group (string label) {
	this->setLabel(label);
}

void Group::addMember (PartBase *member) {
	if (members.size() == 0)
		boundingBox = member->boundingBox;

	boundingBox += member->boundingBox;

	// Insert label in labelIndexMap
	labelIndexMap.insert(make_pair(member->label, members.size()));

	members.push_back(member);
}

void Group::render (DisplayType displayType) {
	renderBoundingBox();
	for (vector<PartBase*>::iterator it = members.begin() ; it != members.end(); ++it) {
		PartBase* member = *it;

		member->render(displayType);
	}
}

void Group::renderForProjection (double scale, Vector center) {
	for (vector<PartBase*>::iterator it = members.begin() ; it != members.end(); ++it) {
		PartBase* member = *it;

		member->renderForProjection(scale, center);
	}	
}

PartBase *Group::getMember (string label) {
	if (labelIndexMap.count(label) > 0)
		return members[labelIndexMap[label]];
	else
		return NULL;
}

void Group::setMember (string label, PartBase *member) {
	if (labelIndexMap.count(label) > 0)
		members[labelIndexMap[label]] = member;
}

#endif