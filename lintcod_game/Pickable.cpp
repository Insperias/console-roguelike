#include "pch.h"
#include "Pickable.h"


Pickable::Pickable()
{
}


Pickable::~Pickable()
{
}

bool Pickable::pick(Actor * owner, Actor * wearer)
{
	if (wearer->container && wearer->container->add(owner)) {
		engine.actors.remove(owner);
		return true;
	}
	return false;
}

bool Pickable::use(Actor * owner, Actor * wearer)
{
	if (wearer->container) {
		wearer->container->remove(owner);
		delete owner;
		return true;
	}
	return false;
}

void Pickable::drop(Actor * owner, Actor * wearer)
{
	if (wearer->container) {
		wearer->container->remove(owner);
		engine.actors.push(owner);
		owner->set_x_pos(wearer->get_x_pos());
		owner->set_y_pos(wearer->get_y_pos());
		engine.gui->message(TCODColor::lightGrey, "%s drops a %s",
			wearer->get_name(), owner->get_name());
	}
}

Healer::Healer(float amount) : amount(amount)
{
}

bool Healer::use(Actor * owner, Actor * wearer)
{
	if (wearer->destructible) {
		float amountHealed = wearer->destructible->heal(amount);
		if (amountHealed > 0) {
			return Pickable::use(owner, wearer);
		}
	}
	return false;
}

DamageSpell::DamageSpell(float range, float damage) : range(range), damage(damage)
{
}

LightningBolt::LightningBolt(float range, float damage)
	: DamageSpell(range, damage)
{
}

bool LightningBolt::use(Actor * owner, Actor * wearer)
{
	Actor *closestMonster = engine.getClosetMonster(wearer->get_x_pos(),
		wearer->get_y_pos(), this->range);
	if (!closestMonster) {
		engine.gui->message(TCODColor::lightGrey, "No enemy is close enought to strike");
		return false;
	}
	//hit closest monster
	engine.gui->message(TCODColor::lightBlue,
		"A lighting bolt strikes the %s with a loud thunder!\n"
		"The damage is %g hp.", closestMonster->get_name(), this->damage);
	closestMonster->destructible->takeDamage(closestMonster, this->damage);
	return Pickable::use(owner, wearer);
}


Fireball::Fireball(float range, float damage) 
	: DamageSpell(range, damage)
{
}

bool Fireball::use(Actor * owner, Actor * wearer)
{
	engine.gui->message(TCODColor::cyan, "Left-click a target tile for the fireball,\nor right-click to cancel");
	int x, y;
	if (!engine.pickATile(&x, &y)) {
		return false;
	}
	//burn everything in some range (player including)
	engine.gui->message(TCODColor::orange, "The fireball explodes, burning everythin within %g tiles!", this->range);
	for (Actor **it = engine.actors.begin();
		it != engine.actors.end(); it++) {
		Actor *actor = *it;
		if (actor->destructible && !actor->destructible->isDead()
			&& actor->getDistance(x, y) <= this->range) {
			engine.gui->message(TCODColor::orange, "The %s gets burned for %g hp",
				actor->get_name(), this->damage);
			actor->destructible->takeDamage(actor, this->damage);
		}
	}
	return Pickable::use(owner, wearer);
}

Confuser::Confuser(int nbTurns, float range) 
	: nbTurns(nbTurns), range(range)
{
}

bool Confuser::use(Actor * owner, Actor * wearer)
{
	engine.gui->message(TCODColor::cyan, "Left-click an enemy to confuse it,\nor right-click to cancel");
	int x, y;
	if (!engine.pickATile(&x, &y, range)) {
		return false;
	}

	Actor *actor = engine.getActor(x, y);
	if (!actor) {
		return false;
	}
	//confuse the monster(or player) for <nbTurns> turns
	Ai *confusedAi = new ConfusedMonsterAi(nbTurns, actor->ai);
	actor->ai = confusedAi;
	engine.gui->message(TCODColor::lightGreen, "The eyes of the %s look vacant,\nas he starts to stumble around!",
		actor->get_name());
	return Pickable::use(owner, wearer);
	
}
