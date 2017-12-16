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

#define HAUSDORFF_THRESHOLD 0.2

using std::vector;
using std::map;
using std::make_pair;
using std::pair;

enum Transformation_Type {UNIFORM, NONUNIFORM};

class Group : public PartBase {
public:
	vector<PartBase*> members;
	map<string, int> labelIndexMap;

	bool has_color = false;

	Group (string label);
	~Group () {};

	void addMember (PartBase *member);
	void render (DisplayType displayType) override;
	void renderForProjection (double scale, Vector center) override;
	int writeToFile (ofstream& smf_file, int vertexStartIndex, Transformation t) override;
	void startWriteToFile (string filename);

	PartBase *getMember (string label);
	void setMember (string label, PartBase *member);
	
	bool removeMember(string label1);
	PartBase *getMemberGlobally (string label);			
	PartBase *getParentGlobally (string label);
	void print (int level);		
    PartBase* make_copy();		
    bool replace(string label1, PartBase *part2, string label2);
    bool partByPartSwap (string label1, PartBase *p2, string label2);		
    bool swap(string label1, PartBase *part2, string label2);

    // Transformation
    void applyTransformation (Transformation transform) override;
    void transformTo (PartBase *part2, Transformation_Type type);

    bool hasArmRest();

    void setColor (int color_index);
    void setColor (Vector c);
};

Group::Group (string label) {
	this->setLabel(label + "_Group");
}

void Group::setColor (int color_index) {

	this->c_index = color_index;
	for(int i = 0; i < this->members.size(); i++)	
	{
		
		//std::cout << "set color (group): " << this->members[i]->label << "(" << color_index << ")" << std::endl;
		this->members[i]->setColor(color_index);		
	}


	// if(color_index < color_count)	
	// {		
	// 	for(int i = 0; i < this->members.size(); i++)			
	// 			this->members[i]->setColor(color_index);		
	// } else
	// {
	// 	if(!this->has_color)
	// 	{
	// 		this->color = getRandomColor();
	// 		this->has_color = true;
	// 	}
	// 	this->setColor(this->color);

	// }
};	

void Group::setColor (Vector c) {	
	PartBase::setColor(c);
	
	for(int i = 0; i < this->members.size(); i++)
		this->members[i]->setColor(c);
	
};	

void Group::addMember (PartBase *member) {
	if (members.size() == 0)
		boundingBox = member->boundingBox;

	boundingBox += member->boundingBox;

	// Insert label in labelIndexMap
	//labelIndexMap.insert(make_pair(member->label, members.size()));
	labelIndexMap[member->label] = members.size();

	members.push_back(member);
}

void Group::render (DisplayType displayType) {
	// renderBoundingBox();
	glPushMatrix();
	glMultMatrixf(getTransformationArray());

	for (vector<PartBase*>::iterator it = members.begin() ; it != members.end(); ++it) {
		PartBase* member = *it;

		member->render(displayType);
	}

	glPopMatrix();
}

void Group::renderForProjection (double scale, Vector center) {
	glPushMatrix();
	glMultMatrixf(getTransformationArray());

	for (vector<PartBase*>::iterator it = members.begin() ; it != members.end(); ++it) {
		PartBase* member = *it;

		member->renderForProjection(scale, center);
	}

	glPopMatrix();
}

PartBase *Group::getMember (string label) {
	if (labelIndexMap.count(label) > 0)
		return members[labelIndexMap[label]];
	else
		return NULL;
}

PartBase *Group::getMemberGlobally (string label) {

	// std::cout << "\t" << this->label << endl;
	// for (std::map<string,int>::iterator it=labelIndexMap.begin(); it!=labelIndexMap.end(); ++it)
 //    	std::cout << "\t\t" << it->first << " - " << it->second << endl;


    // if(this->label == label)
    // 	return this;

	if (labelIndexMap.count(label) > 0) {		
		return members[labelIndexMap[label]];
	}
	else {	
		for (std::map<string,int>::iterator it=labelIndexMap.begin(); it!=labelIndexMap.end(); ++it)
			//if(boost::algorithm::ends_with(it->first, "_Group")) {			
			if(Group* theGroup = dynamic_cast<Group*>(this->members[it->second])) {		
				//PartBase *theGroup = this->members[it->second];
				PartBase *thePart = theGroup->getMember(label);
				if(NULL != thePart) 
					return thePart;
			}

		return NULL;
	}
}

PartBase *Group::getParentGlobally (string label) {	

	if (labelIndexMap.count(label) > 0) {			
		return this;
	}
	else {	
		for (std::map<string,int>::iterator it=labelIndexMap.begin(); it!=labelIndexMap.end(); ++it)
			//if(boost::algorithm::ends_with(it->first, "_Group")) {			
			if(Group* theGroup = dynamic_cast<Group*>(this->members[it->second])) {				
				PartBase *thePart = theGroup->getParentGlobally(label);
				if(NULL != thePart) 
					return thePart;
			}

		return NULL;
	}
}


void Group::print (int level) {

	for(int i = 0; i < level; i++)
		std::cout << "\t";

	//std::cout << this->label << ": (" << this->color[0] << ", " << this->color[1] << ", " << this->color[2] << std::endl;
	std::cout << this->label << ":" << std::endl;

	
	if(Group* theChair = dynamic_cast<Group*>(this)){

		for (std::map<string,int>::iterator it=labelIndexMap.begin(); it!=labelIndexMap.end(); ++it)
		{			
			PartBase *thePart = this->members[it->second];
			if(Group* theObj = dynamic_cast<Group*>(thePart))
				theObj->print(level+1);
			else {
				for(int i = 0; i < level+1; i++)
					std::cout << "\t";

				std::cout << thePart->label << ": " << std::endl;
			}
		}

	}
	
}


PartBase* Group::make_copy() {

	// 1. copy the object itself

	//string label;
	//Vector color;
	//BoundingBox boundingBox;

	//vector<PartBase*> members;
	//map<string, int> labelIndexMap;
	
	string theLabel = this->label;
	int s_idx = this->label.find("_Group");
	theLabel = theLabel.erase(s_idx, 6);

	Group *aGroup = new Group(theLabel);

	
	// 2. copy children


	for (std::map<string,int>::iterator it=labelIndexMap.begin(); it!=labelIndexMap.end(); ++it)
	{			
		PartBase *thePart = this->members[it->second];
		aGroup->addMember(thePart->make_copy());
	}


	return aGroup;
}

bool Group::removeMember(string label) {

	if (this->labelIndexMap.count(label) <= 0)  return false;


	int idx = this->labelIndexMap[label];
	// reallocate memory	
	// delete this->members[idx];	
	this->members.erase(this->members.begin()+idx);

	//this->labelIndexMap.erase(label);

	// build map again properly because it contains the index information
	this->labelIndexMap.clear();
	for (int i = 0; i < this->members.size(); i++)
		this->labelIndexMap[this->members[i]->label] = i;


	// todo: we need to recompute the bounding box
	this->boundingBox = *(new BoundingBox());
	for (int i = 0; i < this->members.size(); i++)
	{
		PartBase *theMember = this->members[i];

		if (members.size() == 0)
			this->boundingBox = theMember->boundingBox;

		this->boundingBox += theMember->boundingBox;
	}
	
	
	return true;

}

bool Group::replace(string label1, PartBase *part2, string label2)
{	
		PartBase *parent = this->getParentGlobally(label1);		

		// 2. make a copy of the element in part2 with label 2
		PartBase *targetPart = part2->getMemberGlobally(label2);
		PartBase *copyElement = targetPart->make_copy();

		PartBase *sourcePart = this->getMemberGlobally(label1);		

		// 1. remove the element from part1 with label1
		if(!this->removeMember(label1)) 
		{ 
			return false;
		}	

		// 3. insert the copied element into the part1
		
		if(Group* theParent = dynamic_cast<Group*>(parent)) 
		{
			Group *copyGroup = dynamic_cast<Group*>(copyElement);
			Group *sourceGroup = dynamic_cast<Group*>(sourcePart);
			
			if ((copyGroup != NULL) && (sourceGroup != NULL)) {
				copyGroup->transformTo(sourceGroup, NONUNIFORM);
				theParent->addMember(copyElement);
			} else {
				theParent->addMember(copyElement);
			}
			
			return true;
		}

		// 4. recompute the bounding box
		
		return false;
}

bool Group::partByPartSwap (string label1, PartBase *p2, string label2) {
	Group *group1 = dynamic_cast<Group*> (this->getMemberGlobally(label1));
	Group *group2 = dynamic_cast<Group*> (p2->getMemberGlobally(label2));

	if (group1 == NULL || group2 == NULL)
		return false;

	vector<pair<PartBase*, PartBase*>> pairs; 
	for (PartBase *p1: group1->members) {
		for (PartBase *p2: group2->members) {
			Part *part1 = (Part*) p1;
			Part *part2 = (Part*) p2;
			float hausdorffDistance = getHausdorffDistance(part1->mesh, part2->mesh);
			if (hausdorffDistance < 0.25) {
				pairs.push_back(make_pair(p1, p2));
			}
		}
	}

	int count = 0;
	for (pair<PartBase*, PartBase*> pair : pairs) {
		Part *part1 = (Part*) pair.first->make_copy();
		Part *part2 = (Part*) pair.second->make_copy();
		part2->transformTo(part1);
		part1->transformTo(part2);
		this->getMember(label1)->setMember(part1->label, part2);
		p2->getMember(label2)->setMember(part2->label, part1);
		count++;
	}

	return count > 0;
}

bool Group::swap(string label1, PartBase *part2, string label2)
{
	PartBase *copyPart1 = this->make_copy();

	bool partByPartSwapPerformed = partByPartSwap(label1, part2, label2);

	if (!partByPartSwapPerformed) {
		this->replace(label1, part2, label2);
		part2->replace(label2, copyPart1, label1);	
	}
}

void Group::setMember (string label, PartBase *member) {
	if (labelIndexMap.count(label) > 0)
		members[labelIndexMap[label]] = member;
}

int Group::writeToFile (ofstream& smf_file, int vertexStartIndex, Transformation t) {
	Transformation total_transformation = transformation * t;

	int vertexIndex = vertexStartIndex;	
	for (vector<PartBase*>::iterator it = members.begin() ; it != members.end(); ++it) {
		PartBase* member = *it;

		vertexIndex = member->writeToFile(smf_file, vertexIndex, total_transformation);
	}

	return vertexIndex;
}

void Group::startWriteToFile (string filename) {
	ofstream smf_file;
	smf_file.open(filename.c_str());

	writeToFile(smf_file, 1, transformation);

	smf_file.close();

}

// Transformation
void Group::applyTransformation (Transformation transform) {
	for (vector<PartBase*>::iterator it = members.begin() ; it != members.end(); ++it) {
		PartBase* member = *it;

		member->applyTransformation(transform);
	}
};

void Group::transformTo (PartBase *part2, Transformation_Type type) {
	switch (type) {
		case UNIFORM:
			this->applyTransformation(getTransformation(part2->boundingBox, this->boundingBox));
			break;
		case NONUNIFORM:
			this->applyTransformation(getNonUniformTransformation(part2->boundingBox, this->boundingBox));
			break;
	}
}

bool Group::hasArmRest () {
	PartBase* armRestGroup = this->getMemberGlobally("Arm_Group");
	if(Group* group = dynamic_cast<Group*>(armRestGroup)) 
	{	
		if(group->members.size() > 0)
			return true;
	}
	return false;
}

#endif