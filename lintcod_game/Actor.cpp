#include "pch.h"


Actor::Actor()
{
	this->x = 40;
	this->y = 25;
	this->ch = '@';
	this->col = TCODColor(230, 144, 78);
}

Actor::Actor(int x, int y, int ch,const char * name, const TCODColor & col): x(x), y(y), ch(ch), name(name), col(col),
	blocks(true), attacker(nullptr), destructible(nullptr), ai(nullptr), pickable(nullptr), container(nullptr), fovOnly(true)
{
}

Actor::~Actor()
{
	if (attacker) delete attacker;
	if (destructible) delete destructible;
	if (ai) delete ai;
	if (pickable) delete pickable;
	if (container) delete container;
}

void Actor::update()
{
	if (ai) ai->update(this);
}


void Actor::render() const
{
	TCODConsole::root->setChar(x, y, ch);
	TCODConsole::root->setCharForeground(x, y, col);
}

float Actor::getDistance(int cx, int cy) const
{
	int dx = this->x - cx;
	int dy = this->y - cy;
	return sqrtf(dx * dx + dy * dy);
}

void Actor::load(TCODZip & zip)
{
	x = zip.getInt();
	y = zip.getInt();
	ch = zip.getInt();
	col = zip.getColor();
	name = _strdup(zip.getString());
	blocks = zip.getInt();

	bool hasAttacker = zip.getInt();
	bool hasDestructible = zip.getInt();
	bool hasAi = zip.getInt();
	bool hasPickable = zip.getInt();
	bool hasContainer = zip.getInt();

	if (hasAttacker) {
		attacker = new Attacker(0.0f);
		attacker->load(zip);
	}
	if (hasDestructible) {
		destructible = Destructible::create(zip);
	}
	if (hasAi) {
		ai = Ai::create(zip);
	}
	if (hasPickable) {
		pickable = Pickable::create(zip);
	}
	if (hasContainer) {
		container = new Container(0);
		container->load(zip);
	}
}

void Actor::save(TCODZip & zip)
{
	zip.putInt(x);
	zip.putInt(y);
	zip.putInt(ch);
	zip.putColor(&col);
	zip.putString(name);
	zip.putInt(blocks);

	zip.putInt(attacker != nullptr);
	zip.putInt(destructible != nullptr);
	zip.putInt(ai != nullptr);
	zip.putInt(pickable != nullptr);
	zip.putInt(container != nullptr);

	if (attacker) attacker->save(zip);
	if (destructible) destructible->save(zip);
	if (ai) ai->save(zip);
	if (pickable) pickable->save(zip);
	if (container) container->save(zip);

}

int Actor::get_x_pos() const
{
	return this->x;
}

int Actor::get_y_pos() const
{
	return this->y;
}

void Actor::set_x_pos(int x)
{
	this->x = x;
}

void Actor::set_y_pos(int y)
{
	this->y = y;
}

void Actor::set_ch(int ch)
{
	this->ch = ch;
}

int Actor::get_ch() const
{
	return this->ch;
}

void Actor::set_color(TCODColor col)
{
	this->col = col;
}

TCODColor Actor::get_color() const
{
	return this->col;
}

void Actor::set_name(const char * name)
{
	this->name = name;
}

const char * Actor::get_name() const
{
	return this->name;
}
