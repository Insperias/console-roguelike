#include "pch.h"
#include "Container.h"


Container::Container() : size(30)
{
}

Container::Container(int size) : size(size)
{
}


Container::~Container()
{
	inventory.clearAndDelete();
}

bool Container::add(Actor * actor)
{
	if (size > 0 && inventory.size() >= size) {
		return false; //inventory is full
	}

	inventory.push(actor);
	return true;
}

void Container::remove(Actor * actor)
{
	inventory.remove(actor);
}

void Container::load(TCODZip & zip)
{
	size = zip.getInt();
	int nbActors = zip.getInt();
	while (nbActors > 0) {
		Actor *actor = new Actor(0, 0, 0, nullptr, TCODColor::white);
		actor->load(zip);
		inventory.push(actor);
		nbActors--;
	}
}

void Container::save(TCODZip & zip)
{
	zip.putInt(size);
	zip.putInt(inventory.size());
	for (Actor **it = inventory.begin(); it != inventory.end(); it++) {
		(*it)->save(zip);
	}
}
