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

using std::vector;

class Group : public PartBase {
public:
	vector<PartBase*> members;

	Group () {};
	~Group () {};

	void addMember (PartBase *member);
	void render (DisplayType displayType) override;
};

void Group::addMember (PartBase *member) {
	members.push_back(member);
}

void Group::render (DisplayType displayType) {
	for (vector<PartBase*>::iterator it = members.begin() ; it != members.end(); ++it) {
		PartBase* member = *it;

		member->render(displayType);
	}
}

#endif