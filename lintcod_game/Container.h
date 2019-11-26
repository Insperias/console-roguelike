#pragma once
class Container
{
	int size; //max number of actors. 0=unlimited
public:
	TCODList<Actor*> inventory;
	Container();
	Container(int size);
	~Container();
	bool add(Actor* actor);
	void remove(Actor *actor);
};

