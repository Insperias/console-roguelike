#include "pch.h"
#include <stdio.h>


Attacker::Attacker()
{
}

Attacker::Attacker(float power) : power(power)
{
}


Attacker::~Attacker()
{
}

void Attacker::attack(Actor * owner, Actor * target)
{
	if (target->destructible && !target->destructible->isDead()) {
		if (this->power - target->destructible->get_defense() > 0) {
			engine.gui->message(owner == engine.player ? TCODColor::red : TCODColor::lightGrey,
				"%s attacks %s for %g hp.\n", owner->get_name(), target->get_name(),
				this->power - target->destructible->get_defense());
		}
		else {
			engine.gui->message(TCODColor::lightGrey,"%s attacks %s but it has no effect!\n", owner->get_name(), target->get_name());
		}
		target->destructible->takeDamage(target, power);
	}
	else {
		engine.gui->message(TCODColor::lightGrey,"%s ataacks %s in vain. \n", owner->get_name(), target->get_name());
	}
}
