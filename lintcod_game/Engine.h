#pragma once


class Actor;
class Map;

class Engine
{
private:
	bool computeFov;
	int screenWidth;
	int screenHeight;
	TCOD_key_t lastKey;
	TCOD_mouse_t mouse;
	int fovRadius;
public:
	enum GameStatus {
		STARTUP, IDLE, NEW_TURN, VICTORY, DEFEAT
	} gameStatus;
	Gui *gui;
	Actor *player;
	Map *map;
	TCODList<Actor *> actors;
	Engine();
	Engine(int screenWidth, int screenHeight);
	~Engine();
	void sendToBack(Actor *actor);
	void update();
	void render();
	Actor *getClosetMonster(int x, int y, float range) const;
	bool pickATile(int *x, int *y, float maxRange = 0.0f);
	Actor *getActor(int x, int y) const;
	void init();
	void load();
	void save();

	//accessors
	TCOD_key_t get_last_key() const { return this->lastKey; }
	void set_last_key(TCOD_key_t lastKey) { this->lastKey = lastKey; }
	TCOD_mouse_t get_mouse() const { return this->mouse; }
	void set_mouse(TCOD_mouse_t mouse) { this->mouse = mouse; }
	int get_screen_width() const { return this->screenWidth; }
	void set_screen_width(int screenWidth) { this->screenWidth = screenWidth; }
	int get_screen_height() const { return this->screenHeight; }
	void set_screen_height(int screenHeight) { this->screenHeight = screenHeight; }
	int get_fov_radius() const { return this->fovRadius; }
	void set_fov_radius(int fovRadius) { this->fovRadius = fovRadius; }
};

extern Engine engine;

