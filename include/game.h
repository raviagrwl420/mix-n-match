
#ifndef GAME_H
#define GAME_H


#include <gl.h>

#include <GL/glut.h>
#include <GL/glui.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <parser.h>
#include <part.h>
#include <group.h>
#include <scorer.h>
#include <cgal.h>
#include "RelationContainer.h"
#include "relation.h"


const int GAME_SWAP_LEGS = 0;
const int GAME_SWAP_BACK = 1;
const int GAME_SWAP_SEAT = 2;
const int GAME_SWAP_ARM  = 3;

class Game {	
public:
	// we play for 10 round
	int round = 10;
	int current_round = 1;
	// a round is completed if user selected 3 new good chairs, then we move to the next round
	int selection_num = 3;

	int original_chairs_num;
	
	vector<PartBase*> chairs;
	vector<PartBase*> old_unused_chairs;

	// we randomly select two chairs from "chairs"
	PartBase *chairA;
	PartBase *chairB;

	vector<PartBase*> selected_charis;

	Game (string path) {
		this->chairs = loadFiles(path);

		for(int i = 0; i < this->chairs.size(); i++)
			this->chairs[i]->setColor(i);			

		this->original_chairs_num = this->chairs.size();
	}

	static Game* initGame (string path)
	{		
		return new Game(path);
	}

	int getNewChairsNum()
	{
		this->chairs.size() - this->original_chairs_num;
	}

	void run()
	{
		this->select_randomly();
	}

	PartBase *getChairA()
	{
		return this->chairA;
	}

	PartBase *getChairB()
	{
		return this->chairB;
	}

	void select_randomly() {
		int index_a = rand() % this->chairs.size();
		int index_b = rand() % this->chairs.size();
		while(index_a == index_b)
			index_b = rand() % this->chairs.size();		

		PartBase *chair_1 = chairs[index_a];
		PartBase *chair_2 = chairs[index_b];	

		// create two chairs by randomly swap the parts of selected chairs
		this->do_swap(chair_1, chair_2);

	}

	void do_swap(PartBase *theChair1, PartBase *theChair2) 
	{

		Group* c1 = dynamic_cast<Group*>(theChair1);
		Group* c2 = dynamic_cast<Group*>(theChair2);

		bool hasArm_1 = c1->hasArmRest();
		bool hasArm_2 = c2->hasArmRest();

		PartBase* copy_1 = c1->make_copy();
		PartBase* copy_2 = c2->make_copy();




		int action = rand() % (3 + ( (hasArm_1 || hasArm_2)?1:0 ));

		if(GAME_SWAP_ARM == action)
		{
			if(hasArm_1 && hasArm_2)
				_swap(action, copy_1, copy_2);

			else if(hasArm_1)
			{				
				// chair 1 has arm so give it to 2
				_replace(action, copy_2, copy_1);


				// do something for part 1 as well
				int to_act_for_1 = rand() % 3;
				_replace(to_act_for_1, copy_1, copy_2);

			}
			else if(hasArm_2)
			{
				// chair 2 has arm so give it to 1
				_replace(action, copy_1, copy_2);


				// do something for part 2 as well
				int to_act_for_2 = rand() % 3;
				_replace(to_act_for_2, copy_2, copy_1);
			}


		} else _swap(action, copy_1, copy_2);		
		

		this->chairA = copy_1;
		this->chairB = copy_2;			
	}

	void _swap(int action, PartBase* chair1, PartBase* chair2)
	{
		switch(action)
		{
			case GAME_SWAP_LEGS:
				chair1->swap("Leg_Group", chair2, "Leg_Group");
			break;

			case GAME_SWAP_BACK:
				chair1->swap("Back_Group", chair2, "Back_Group");
			break;

			case GAME_SWAP_SEAT:
				chair1->swap("Seat_Group", chair2, "Seat_Group");
			break;

			case GAME_SWAP_ARM:
				chair1->swap("Arm_Group", chair2, "Arm_Group");

			break;
		}
	}

	void _replace(int action, PartBase* chair1, PartBase* chair2)
	{
		switch(action)
		{
			case GAME_SWAP_LEGS:				
				chair1->replace("Arm_Group", chair2, "Arm_Group");
			break;

			case GAME_SWAP_BACK:				
				chair1->replace("Back_Group", chair2, "Back_Group");
			break;

			case GAME_SWAP_SEAT:				
				chair1->replace("Seat_Group", chair2, "Seat_Group");
			break;

			case GAME_SWAP_ARM:				
				chair1->replace("Arm_Group", chair2, "Arm_Group");
			break;
		}
	}

	bool do_select(string selection) {	
		// todo: release every memory that doesn't important anymore
		this->release_unused_memory();
		if("A" == selection)
		{	
			this->selected_charis.push_back(this->chairA);			
			this->old_unused_chairs.push_back(this->chairB);
		}
		if("B" == selection)		
		{
			this->selected_charis.push_back(this->chairB);
			this->old_unused_chairs.push_back(this->chairA);
		}
		if("Both" == selection)	
		{	
			this->selected_charis.push_back(this->chairA);
			this->selected_charis.push_back(this->chairB);
		}

		if(this->selection_num <= this->selected_charis.size())
			return this->next_round();
		else this->select_randomly();

		return true;
	}

	void release_unused_memory()
	{
		for(int i = 0; i < this->old_unused_chairs.size(); i++)
			delete this->old_unused_chairs[i];

		this->old_unused_chairs.clear();
	}

	bool next_round()
	{		
		this->chairs.insert(std::end(this->chairs), std::begin(this->selected_charis), std::end(this->selected_charis));
		this->selected_charis.clear();
		this->current_round++;
		if(this->current_round > this->round)
		{		
			this->game_done();
			return false;
		}
		else this->select_randomly();

		return true;		
	}


	void game_done()
	{
		// todo: save the game
	}

	void do_save(string chair)
	{
		if ("A" == chair) {
			((Group*) chairA)->startWriteToFile(chairA->label + ".obj");
		} else if ("B" == chair) {
			((Group*) chairB)->startWriteToFile(chairB->label + ".obj");
		}
	}

};

#endif