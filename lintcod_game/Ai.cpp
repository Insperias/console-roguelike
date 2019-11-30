#include "pch.h"

static const int TRACKING_TURNS = 3;
const int LEVEL_UP_BASE = 50;
const int LEVEL_UP_FACTOR = 75;

Ai::Ai()
{
}


Ai::~Ai()
{
}

Ai * Ai::create(TCODZip & zip)
{
	AiType type = (AiType)zip.getInt();
	Ai *ai = nullptr;
	switch (type) {
	case PLAYER: ai = new PlayerAi(); break;
	case MONSTER: ai = new MonsterAi(); break;
	case CONFUSED_MONSTER: ai = new ConfusedMonsterAi(0, nullptr); break;
	}
	ai->load(zip);
	return ai;
}



PlayerAi::PlayerAi() : xpLevel(1)
{
}

void PlayerAi::update(Actor * owner)
{
	int levelUpXp = getNextLevelXp();
	if (owner->destructible->get_xp() >= levelUpXp) {
		xpLevel++;
		owner->destructible->set_xp(owner->destructible->get_xp() - levelUpXp);

		engine.gui->message(TCODColor::yellow, "You skills grow stronger! You reached level %d", xpLevel);
		engine.gui->menu.clear();
		engine.gui->menu.addItem(Menu::CONSTITUTION, "Constitution (+15HP)");
		engine.gui->menu.addItem(Menu::STRENGTH, "Strength (+1 attack)");
		engine.gui->menu.addItem(Menu::AGILITY, "Agility (+1 defense)");
		Menu::MenuItemCode menuItem = engine.gui->menu.pick(Menu::PAUSE);

		switch (menuItem) {
		case Menu::CONSTITUTION:
			owner->destructible->set_maxHp(owner->destructible->get_maxHp() + 15);
			owner->destructible->set_hp(owner->destructible->get_hp() + 20);
			if (owner->destructible->get_hp() > owner->destructible->get_maxHp()) {
				owner->destructible->set_hp(owner->destructible->get_maxHp());
			}
			break;
		case Menu::STRENGTH:
			owner->attacker->set_power(owner->attacker->get_power() + 1);
			break;
		case Menu::AGILITY:
			owner->destructible->set_defense(owner->destructible->get_defense() + 1);
			break;
		default:break;
			
			if (xpLevel % 2) {
				for (Actor **it = engine.actors.begin();
					it != engine.actors.end(); it++) {
					Actor *actor = *it;
					if (actor != owner && actor->destructible) {
						actor->attacker->set_power(actor->attacker->get_power() + 2);
						actor->destructible->set_maxHp(actor->destructible->get_maxHp() + 20);
						actor->destructible->set_defense(actor->destructible->get_defense() + 1.5f);
						actor->destructible->set_xp(actor->destructible->get_xp() + 7);
					}
					if (actor != owner && actor->pickable)
					{
						if (actor->pickable->get_type() == actor->pickable->DAMAGE_SPELL) {
							DamageSpell *spell = (DamageSpell*)actor->pickable;
							spell->set_damage(spell->get_damage() + 7);
							actor->pickable = spell;
						}
						else if (actor->pickable->get_type() == actor->pickable->HEAL_SPELL) {
							Healer* spell = (Healer*)actor->pickable;
							spell->set_amount(spell->get_amount() + 3);
							actor->pickable = spell;
						}

					}
				}
			}
		}
	}
	if (owner->destructible && owner->destructible->isDead()) {
		return;
	}

	int dx = 0, dy = 0;
	switch (engine.get_last_key().vk) { //player move
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
	case TCODK_CHAR :
		handleActionKey(owner, engine.get_last_key().c);
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

void PlayerAi::handleActionKey(Actor * owner, int ascii)
{
	switch (ascii) {
	case 'g' : //pickup item
	{
		bool found = false;
		for (Actor **iterator = engine.actors.begin();
			iterator != engine.actors.end(); iterator++) {
			Actor *actor = *iterator;
			if (actor->pickable && actor->get_x_pos() == owner->get_x_pos() &&
				actor->get_y_pos() == owner->get_y_pos()) {
				if (actor->pickable->pick(actor, owner)) {
					found = true;
					engine.gui->message(TCODColor::lightGrey, "You pick up the %s.",
						actor->get_name());
					//engine.gameStatus = Engine::NEW_TURN;
					break;
				}
				else if (!found) {
					found = true;
					engine.gui->message(TCODColor::red, "Your inventory is full.");
				}
			}
		}
		if (!found) {
			engine.gui->message(TCODColor::lightGrey, "There's nothing here that you can pick up.");
			engine.gameStatus = Engine::NEW_TURN;
		}
		break;
	}
	case 'i': //display inventory
	{
			this->inventory_status = PlayerAi::OPEN;
			while (inventory_status == PlayerAi::OPEN)
			{
				Actor *actor = choseFromInventory(owner);
				if (actor) {
					bool spell = actor->get_name() != "scroll of heal";
					if (spell) {
						this->inventory_status = PlayerAi::CLOSE;
					}
					actor->pickable->use(actor, owner);
					engine.gameStatus = Engine::NEW_TURN;
				}
				
			}
			break;

	}
	case 'd': //drop item
	{
		this->inventory_status = PlayerAi::OPEN;
		while (inventory_status == PlayerAi::OPEN)
		{
			Actor *actor = choseFromInventory(owner);
			if (actor) {
				actor->pickable->drop(actor, owner);
				engine.gameStatus = Engine::NEW_TURN;
			}
		}
		break;
	}
	case '>'://next lvl dungeon
		if (engine.stairs->get_x_pos() == owner->get_x_pos()
			&& engine.stairs->get_y_pos() == owner->get_y_pos()) {
			engine.nextLevel();
		}
		else {
			engine.gui->message(TCODColor::lightGrey, "There are no stairs here");
		}
		break;
	}
	
}

void PlayerAi::load(TCODZip & zip)
{

}

void PlayerAi::save(TCODZip & zip)
{
	zip.putInt(PLAYER);
}

int PlayerAi::getNextLevelXp()
{
	return LEVEL_UP_BASE + xpLevel * LEVEL_UP_FACTOR;
}

bool PlayerAi::moveOrAttack(Actor * owner, int targetx, int targety)
{
	if (engine.map->isWall(targetx, targety)) return false;
	//look for living actors to attack
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->destructible && !actor->destructible->isDead()
			&& actor->get_x_pos() == targetx && actor->get_y_pos() == targety) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	//look for corpses or items
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		bool corpseOrItem = (actor->destructible && actor->destructible->isDead())
			|| actor->pickable;
		if (corpseOrItem
			&& actor->get_x_pos() == targetx && actor->get_y_pos() == targety) {
			engine.gui->message(TCODColor::lightGrey,"There's a %s here\n", actor->get_name());
		}
	}

	
	owner->set_x_pos(targetx);
	owner->set_y_pos(targety);
	return true;
}

Actor * PlayerAi::choseFromInventory(Actor * owner)
{
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);

	//didplay the inventory frame
	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true,
		TCOD_BKGND_DEFAULT, "inventory");

	//display the items with their kb shortcut
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	int y = 1;
	for (Actor **it = owner->container->inventory.begin();
		it != owner->container->inventory.end(); it++) {
		Actor *actor = *it;
		con.printf(2, y, "(%c) %s", shortcut, actor->get_name());
		y++;
		shortcut++;
	}
	//blit the inventory console on the console
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT,
		TCODConsole::root, engine.get_screen_width() / 2 - INVENTORY_WIDTH / 2,
		engine.get_screen_height() / 2 - INVENTORY_HEIGHT / 2);
	TCODConsole::flush();

	//w8 a key press
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, nullptr, true);
	if (key.vk == TCODK_CHAR) {
		if (key.c == 'i') {
			this->inventory_status = PlayerAi::CLOSE;
			return nullptr;
		}
		int actorIndex = key.c - 'a';
		if (actorIndex >= 0 && actorIndex < owner->container->inventory.size()) {
			return owner->container->inventory.get(actorIndex);
		}
	}
	return nullptr;
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

void MonsterAi::load(TCODZip & zip)
{
	moveCount = zip.getInt();
}

void MonsterAi::save(TCODZip & zip)
{
	zip.putInt(MONSTER);
	zip.putInt(moveCount);
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

ConfusedMonsterAi::ConfusedMonsterAi(int nbTurns, Ai * oldAi)
	: nbTurns(nbTurns), oldAi(oldAi)
{
}

void ConfusedMonsterAi::update(Actor * owner)
{
	TCODRandom *rng = TCODRandom::getInstance();
	int dx = rng->getInt(-1, 1);
	int dy = rng->getInt(-1, 1);

	if (dx != 0 || dy != 0) {
		int destx = owner->get_x_pos() + dx;
		int desty = owner->get_y_pos() + dy;
		if (engine.map->canWalk(destx, desty)) {
			owner->set_x_pos(destx);
			owner->set_y_pos(desty);
		}
		else {
			Actor *actor = engine.getActor(destx, desty);
			if (actor) {
				owner->attacker->attack(owner, actor);
			}
		}
	}
	this->nbTurns--;
	if (this->nbTurns == 0) {
		owner->ai = oldAi;
		delete this;
	}
}

void ConfusedMonsterAi::load(TCODZip & zip)
{
	nbTurns = zip.getInt();
	oldAi = Ai::create(zip);
}

void ConfusedMonsterAi::save(TCODZip & zip)
{
	zip.putInt(CONFUSED_MONSTER);
	zip.putInt(nbTurns);
	oldAi->save(zip);
}
