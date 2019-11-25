#include "pch.h"

Engine::Engine() : fovRadius(10), gameStatus(STARTUP)
{
	TCODConsole::initRoot(80, 50, "dungeon hero", false);
	this->player = new Actor(40, 25, '@', "player", TCODColor::white);
	this->actors.push(player);
	this->map = new Map(80, 43);
	this->gui = new Gui();
}

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(10),
	screenWidth(screenWidth), screenHeight(screenHeight)
{
	TCODConsole::initRoot(screenWidth, screenHeight, "dungeoun hero", false);
	this->player = new Actor(40, 25, '@', "player", TCODColor::white);
	this->player->destructible = new PlayerDestructible(30, 3, "your cadaver");
	this->player->attacker = new Attacker(5);
	this->player->ai = new PlayerAi();
	this->actors.push(player);
	this->map = new Map(screenWidth, screenHeight - 5);
	this->gui = new Gui();
	this->gui->message(TCODColor::orange, "Welcome stranger!\nPrepare to perish in the Cyber Heroes Kingdom.");
	
}


Engine::~Engine()
{
	actors.clearAndDelete();
	delete map;
	delete gui;
}

void Engine::sendToBack(Actor * actor)
{
	this->actors.remove(actor);
	this->actors.insertBefore(actor, 0);
}

void Engine::update()
{
	if (gameStatus == STARTUP) map->computeFov();
	gameStatus = IDLE;
	TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS | TCOD_EVENT_MOUSE, &lastKey, &mouse);
	this->player->update();


	if (gameStatus == NEW_TURN) {
		for (Actor **iterator = actors.begin(); iterator != actors.end();
			iterator++) {
			Actor *actor = *iterator;
			if (actor != player) {
				actor->update();
			}
		}
	}
}

void Engine::render()
{
	TCODConsole::root->clear();
	
	//draw the map
	map->render();

	int size = actors.size();
	//draw the actors
	for (Actor **iterator = actors.begin(); iterator != actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (this->map->isInFov(actor->get_x_pos(), actor->get_y_pos())) {
			actor->render();
		}
	}

	player->render();
	
	//draw GUI
	gui->render();
}
