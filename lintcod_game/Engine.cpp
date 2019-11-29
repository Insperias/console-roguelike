#include "pch.h"
#pragma warning(disable : 4996)

Engine::Engine() : fovRadius(10), gameStatus(STARTUP)
{
	TCODConsole::initRoot(80, 50, "dungeon hero", false);
	this->player = new Actor(40, 25, '@', "player", TCODColor::white);
	this->actors.push(player);
	this->map = new Map(80, 43);
	this->gui = new Gui();
}

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(10),
	screenWidth(screenWidth), screenHeight(screenHeight), player(nullptr), map(nullptr)
{
	TCODConsole::initRoot(screenWidth, screenHeight, "Hidden Dungeons of Balgrad", false);
	this->gui = new Gui();
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

Actor * Engine::getClosetMonster(int x, int y, float range) const
{
	Actor *closest = nullptr;
	float bestDistance = 1E6f;

	for (Actor **it = actors.begin(); it != actors.end(); it++) {
		Actor *actor = *it;
		if (actor != player && actor->destructible
			&& !actor->destructible->isDead()) {
			float distance = actor->getDistance(x, y);
			if (distance < bestDistance && (distance <= range
				|| range == 0.0f)) {
				bestDistance = distance;
				closest = actor;
			}
		}
	}
	return closest;
}

bool Engine::pickATile(int * x, int * y, float maxRange)
{
	while (!TCODConsole::isWindowClosed()) {
		this->render();
		//highlight the possible range for player
		for (int cx = 0; cx < map->get_width(); cx++) {
			for (int cy = 0; cy < map->get_height(); cy++) {
				if (map->isInFov(cx, cy) && (maxRange == 0
					|| player->getDistance(cx, cy) <= maxRange)) {
					TCODColor col = TCODConsole::root->getCharBackground(cx, cy);
					col = col * 1.2f;
					TCODConsole::root->setCharBackground(cx, cy, col);
				}
			}
		}
		
		TCODSystem::checkForEvent(TCOD_EVENT_MOUSE, nullptr, &mouse);
		if (map->isInFov(mouse.cx, mouse.cy)
			&& (maxRange == 0 || player->getDistance(mouse.cx, mouse.cy) <= maxRange)) {
			TCODConsole::root->setCharBackground(mouse.cx, mouse.cy, TCODColor::white);
			
			if (mouse.lbutton_pressed ) {
				*x = mouse.cx;
				*y = mouse.cy;
				return true;
			}
		}
		if (mouse.rbutton_pressed) {
			return false;
		}
		TCODConsole::flush();
	}
	return false;
}

Actor * Engine::getActor(int x, int y) const
{
	for (Actor **it = actors.begin(); it != actors.end(); it++) {
		Actor *actor = *it;
		if (actor->get_x_pos() == x && actor->get_y_pos() == y
			&& actor->destructible && !actor->destructible->isDead()) {
			return actor;
		}
	}
	return nullptr;
}

void Engine::init()
{
	this->player = new Actor(40, 25, '@', "player", TCODColor::white);
	this->player->destructible = new PlayerDestructible(30, 3, "your cadaver");
	this->player->attacker = new Attacker(5);
	this->player->ai = new PlayerAi();
	this->player->container = new Container(26);
	this->actors.push(player);
	this->map = new Map(screenWidth, screenHeight - 7);
	this->map->init(true);
	this->gui->message(TCODColor::orange, "Welcome stranger!\nPrepare to perish in the Hidden Dungeons of Balgrad.");
}

void Engine::load()
{
	if (TCODSystem::fileExists("game.sav")) {
		TCODZip zip;
		zip.loadFromFile("game.sav");
		//load map
		int width = zip.getInt();
		int height = zip.getInt();
		map = new Map(width, height);
		map->load(zip);
		//load player
		player = new Actor(0, 0, 0, nullptr, TCODColor::white);
		player->load(zip);
		actors.push(player);
		//load other actors
		int nbActors = zip.getInt();
		while (nbActors > 0) {
			Actor *actor = new Actor(0, 0, 0, nullptr, TCODColor::white);
			actor->load(zip);
			actors.push(actor);
			nbActors--;
		}
		//load message log
		gui->load(zip);
	}
	else {
		engine.init();
	}
}

void Engine::save()
{
	if (player->destructible->isDead()) {
		TCODSystem::deleteFile("game.sav");
	}
	else {
		TCODZip zip;
		//save map
		zip.putInt(map->get_width());
		zip.putInt(map->get_height());
		map->save(zip);
		//save player
		player->save(zip);
		//save others actors
		zip.putInt(actors.size() - 1);
		for (Actor **it = actors.begin(); it != actors.end(); it++) {
			if (*it != player) {
				(*it)->save(zip);
			}
		}
		//save message log
		gui->save(zip);
		zip.saveToFile("game.sav");
	}
}
