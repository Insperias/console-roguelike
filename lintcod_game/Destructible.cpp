#include "pch.h"
#include <stdio.h>


Destructible::Destructible()
{
}

Destructible::Destructible(float maxHp, float defense, const char * corpseName, int xp) :
	maxHp(maxHp), hp(maxHp), defense(defense), xp(xp)
{
	this->corpseName = _strdup(corpseName);
}


Destructible::~Destructible()
{
	free((char*)corpseName);
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
	owner->set_block(false);

	engine.sendToBack(owner);
}

float Destructible::heal(float amount)
{
	this->hp += amount;
	if (this->hp > this->maxHp) {
		amount -= this->hp - this->maxHp;
		hp = maxHp;
	}
	return amount;
}

void Destructible::load(TCODZip & zip)
{
	maxHp = zip.getFloat();
	hp = zip.getFloat();
	defense = zip.getFloat();
	corpseName = _strdup(zip.getString());
	xp = zip.getInt();
}

void Destructible::save(TCODZip & zip)
{
	zip.putFloat(maxHp);
	zip.putFloat(hp);
	zip.putFloat(defense);
	zip.putString(corpseName);
	zip.putInt(xp);
}

Destructible * Destructible::create(TCODZip & zip)
{
	DestructibleType type = (DestructibleType)zip.getInt();
	Destructible *destructible = nullptr;
	switch (type) {
	case MONSTER: destructible = new MonsterDestructible(0, 0, nullptr, 0); break;
	case PLAYER: destructible = new PlayerDestructible(0, 0, nullptr); break;
	}
	destructible->load(zip);
	return destructible;
}

MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char * corpseName, int xp) :
	Destructible(maxHp, defense, corpseName, xp)
{
}

void MonsterDestructible::die(Actor * owner)
{
	engine.gui->message(TCODColor::lightGrey,"%s is dead. You gain %d xp\n", 
		owner->get_name(), xp);
	engine.player->destructible->set_xp(engine.player->destructible->get_xp() + xp);
	Destructible::die(owner);
}

void MonsterDestructible::save(TCODZip & zip)
{
	zip.putInt(MONSTER);
	Destructible::save(zip);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char * corpseName) :
	Destructible(maxHp, defense, corpseName, 0)
{
}

void PlayerDestructible::die(Actor * owner)
{
	engine.gui->message(TCODColor::red,"You died\n");
	Destructible::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}

void PlayerDestructible::save(TCODZip & zip)
{
	zip.putInt(PLAYER);
	Destructible::save(zip);
}
