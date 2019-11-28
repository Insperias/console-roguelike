#include "pch.h"


Actor::Actor()
{
	this->x = 40;
	this->y = 25;
	this->ch = '@';
	this->col = TCODColor(230, 144, 78);
}

Actor::Actor(int x, int y, int ch,const char * name, const TCODColor & col): x(x), y(y), ch(ch), name(name), col(col),
	blocks(true), attacker(nullptr), destructible(nullptr), ai(nullptr), pickable(nullptr), container(nullptr)
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
