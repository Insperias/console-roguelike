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

Pickable * Pickable::create(TCODZip & zip)
{
	PickableType type = (PickableType)zip.getInt();
	Pickable *pickable = nullptr;
	switch (type) {
	case HEALER: pickable = new Healer(0); break;
	case LIGHTNING_BOLT: pickable = new LightningBolt(0, 0); break;
	case CONFUSER: pickable = new Confuser(0, 0); break;
	case FIREBALL: pickable = new Fireball(0, 0); break;
	case LIGHTNING_CHAIN: pickable = new LightningChain(0, 0); break;
	}
	pickable->load(zip);
	return pickable;
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

void Healer::load(TCODZip & zip)
{
	amount = zip.getFloat();
}

void Healer::save(TCODZip & zip)
{
	zip.putInt(HEALER);
	zip.putFloat(amount);
}

DamageSpell::DamageSpell(float range, float damage) : range(range), damage(damage)
{
}

void DamageSpell::load(TCODZip & zip)
{
	range = zip.getFloat();
	damage = zip.getFloat();
}

LightningBolt::LightningBolt(float range, float damage)
	: DamageSpell(range, damage)
{
}

bool LightningBolt::use(Actor * owner, Actor * wearer)
{
	/*Actor *closestMonster = engine.getClosetMonster(wearer->get_x_pos(),
		wearer->get_y_pos(), this->range);
	if (!closestMonster) {
		engine.gui->message(TCODColor::lightGrey, "No enemy is close enought to strike");
		return false;
	}
	//hit closest monster
	engine.gui->message(TCODColor::lightBlue,
		"A lighting bolt strikes the %s with a loud thunder!\n"
		"The damage is %g hp.", closestMonster->get_name(), this->damage);
	closestMonster->destructible->takeDamage(closestMonster, this->damage);*/
	engine.gui->message(TCODColor::cyan, "Left-click an enemy for the lightning bolt it,\nor right-click to cancel");
	int x, y;
	if (!engine.pickATile(&x, &y, range)) {
		return false;
	}

	Actor *actor = engine.getActor(x, y);
	if (!actor) {
		return false;
	}
	engine.gui->message(TCODColor::lightBlue,
		"A lightning bolt strikes the %s with a loud thunder!\n"
		"The damage is %g hp.", actor->get_name(), this->damage);
	actor->destructible->takeDamage(actor, this->damage);

	return Pickable::use(owner, wearer);
}

void LightningBolt::save(TCODZip & zip)
{
	zip.putInt(LIGHTNING_BOLT);
	zip.putFloat(range);
	zip.putFloat(damage);
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

void Fireball::save(TCODZip & zip)
{
	zip.putInt(FIREBALL);
	zip.putFloat(range);
	zip.putFloat(damage);
}

Confuser::Confuser(int nbTurns, float range) 
	: DebuffSpell(nbTurns, range)
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

void Confuser::save(TCODZip & zip)
{
	zip.putInt(CONFUSER);
	zip.putInt(nbTurns);
	zip.putFloat(range);
}

DebuffSpell::DebuffSpell(int nbTurns, float range) 
	: nbTurns(nbTurns), range(range)
{
}

void DebuffSpell::load(TCODZip & zip)
{
	nbTurns = zip.getInt();
	range = zip.getFloat();
}

LightningChain::LightningChain(float range, float damage)
	: DamageSpell(range, damage)
{
}

bool LightningChain::use(Actor * owner, Actor * wearer)
{
	engine.gui->message(TCODColor::cyan, "Left-click a target tile for the lightning chain,\nor right-click to cancel");
	int x, y;
	if (!engine.pickATile(&x, &y)) {
		return false;
	}
	Actor *actor = engine.getActor(x, y);
	if (!actor) {
		return false;
	}
	Actor *closestMonster = engine.getClosetMonster(actor->get_x_pos(),
		actor->get_y_pos(), this->range);
	if (!closestMonster) {
		engine.gui->message(TCODColor::blue, "The %s gets burned for %g hp ", 
			actor->get_name(), this->damage);
	}
	else {
		engine.gui->message(TCODColor::blue, "The %s gets burned for %g hp and spawned a chain of lightning",
			actor->get_name(), this->damage);
	}
	actor->destructible->takeDamage(actor, this->damage);
	
	for (Actor **it = engine.actors.begin();
		it != engine.actors.end(); it++) {
		Actor *chain_actor = *it;
		if (chain_actor->destructible && !chain_actor->destructible->isDead()
			&& chain_actor->getDistance(x, y) <= this->range
			&& chain_actor != actor) {
			engine.gui->message(TCODColor::blue, "The %s gets burned for %g hp ",
				chain_actor->get_name(), this->damage);
			chain_actor->destructible->takeDamage(chain_actor, this->damage);
		}
	}
	return Pickable::use(owner, wearer);

}

void LightningChain::save(TCODZip & zip)
{
	zip.putInt(LIGHTNING_CHAIN);
	zip.putFloat(range);
	zip.putFloat(damage);
}
