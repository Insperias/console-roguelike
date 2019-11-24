#pragma once


class Actor;
class Map;
class Engine
{
private:
	bool computeFov;
public:
	enum GameStatus {
		STARTUP, IDLE, NEW_TURN, VICTORY, DEFEAT
	} gameStatus;
	int screenWidth;
	int screenHeight;
	TCOD_key_t lastKey;
	int fovRadius;
	Actor *player;
	Map *map;
	TCODList<Actor *> actors;
	Engine();
	Engine(int screenWidth, int screenHeight);
	~Engine();
	void sendToBack(Actor *actor);
	void update();
	void render();
};

extern Engine engine;

