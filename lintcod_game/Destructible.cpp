#include "pch.h"
#include <stdio.h>


Destructible::Destructible()
{
}

Destructible::Destructible(float maxHp, float defense, const char * corpseName) :
	maxHp(maxHp), hp(maxHp), defense(defense), corpseName(corpseName)
{
}


Destructible::~Destructible()
{
}

float Destructible::takeDamage(Actor * owner, float damage)
{
	damage -= this->defense;
	if (damage > 0) {
		this->hp -= damage;
		if (this->hp <= 0) {
			die(owner);
		}
	}
	else {
		damage = 0;
	}
	return damage;
}

void Destructible::die(Actor * owner)
{
	owner->set_ch('%');
	owner->set_color(TCODColor::darkRed);
	owner->set_name(corpseName);
	owner->blocks = false;

	engine.sendToBack(owner);
}

MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char * corpseName) :
	Destructible(maxHp, defense, corpseName)
{
}

void MonsterDestructible::die(Actor * owner)
{
	printf("%s is dead\n", owner->get_name());
	Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char * corpseName) :
	Destructible(maxHp, defense, corpseName)
{
}

void PlayerDestructible::die(Actor * owner)
{
	printf("You died\n");
	Destructible::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}
