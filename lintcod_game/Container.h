#pragma once
class Container : public Persistent
{
	int size; //max number of actors. 0=unlimited
public:
	TCODList<Actor*> inventory;
	Container();
	Container(int size);
	~Container();
	bool add(Actor* actor);
	void remove(Actor *actor);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

