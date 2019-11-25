#include "pch.h"

static const int TRACKING_TURNS = 3;

Ai::Ai()
{
}


Ai::~Ai()
{
}

void PlayerAi::update(Actor * owner)
{
	if (owner->destructible && owner->destructible->isDead()) {
		return;
	}

	int dx = 0, dy = 0;
	switch (engine.lastKey.vk) { //player move
	case TCODK_UP:
		dy = -1;
		break;
	case TCODK_DOWN:
		dy = 1;
		break;
	case TCODK_LEFT:
		dx = -1;
		break;
	case TCODK_RIGHT:
		dx = 1;
		break;
	default:break;
	}

	if (dx != 0 || dy != 0) {
		engine.gameStatus = Engine::NEW_TURN;
		if (moveOrAttack(owner, owner->get_x_pos() + dx, owner->get_y_pos() + dy)) {
			engine.map->computeFov();
		}
	}
}

bool PlayerAi::moveOrAttack(Actor * owner, int targetx, int targety)
{
	if (engine.map->isWall(targetx, targety)) return false;
	//lool for living actors to attack
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->destructible && !actor->destructible->isDead()
			&& actor->get_x_pos() == targetx && actor->get_y_pos() == targety) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	//look for corpses
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->destructible && actor->destructible->isDead()
			&& actor->get_x_pos() == targetx && actor->get_y_pos() == targety) {
			engine.gui->message(TCODColor::lightGrey,"There's a %s here\n", actor->get_name());
		}
	}
	owner->set_x_pos(targetx);
	owner->set_y_pos(targety);
	return true;
}

void MonsterAi::update(Actor * owner)
{
	if (owner->destructible && owner->destructible->isDead()) {
		return;
	}
	if (engine.map->isInFov(owner->get_x_pos(), owner->get_y_pos())) {
		//can see the player, move to him
		this->moveCount = TRACKING_TURNS;
	}
	else {
		this->moveCount--;
	}
	if (moveCount > 0) {
		moveOrAttack(owner, engine.player->get_x_pos(), engine.player->get_y_pos());
	}
}

void MonsterAi::moveOrAttack(Actor * owner, int targetx, int targety)
{
	int dx = targetx - owner->get_x_pos();
	int dy = targety - owner->get_y_pos();
	int stepdx = (dx > 0 ? 1 : -1);
	int stepdy = (dy > 0 ? 1 : -1);
	float distance = sqrtf(dx * dx + dy * dy);

	if (distance >= 2) {
		dx = (int)(round(dx / distance));
		dy = (int)(round(dy / distance));

		if (engine.map->canWalk(owner->get_x_pos() + dx, owner->get_y_pos() + dy)) {
			owner->set_x_pos(owner->get_x_pos() + dx);
			owner->set_y_pos(owner->get_y_pos() + dy);
		}
	}
	else if (engine.map->canWalk(owner->get_x_pos() + stepdx, owner->get_y_pos())) {
		owner->set_x_pos(owner->get_x_pos() + stepdx);
	}
	else if (engine.map->canWalk(owner->get_x_pos(), owner->get_y_pos() + stepdy)) {
		owner->set_y_pos(owner->get_y_pos() + stepdy);
	}
	else if (owner->attacker) {
		owner->attacker->attack(owner, engine.player);
	}
}
