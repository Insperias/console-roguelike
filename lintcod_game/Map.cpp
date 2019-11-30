#include "pch.h"

static const int MAX_ROOM_MONSTERS = 4;
static const int MAX_ROOM_ITEMS = 3;

Map::Map()
{
}

Map::Map(int width, int height): width(width), height(height)
{
	seed = TCODRandom::getInstance()->getInt(0, 0x7FFFFFFF);
}


Map::~Map()
{
	delete[] this->tiles;
	delete this->map;
}

bool Map::isWall(int x, int y) const
{
	return !this->map->isWalkable(x, y);
}

bool Map::canWalk(int x, int y) const
{
	if (isWall(x, y)) {
		return false;
	}
	for (Actor **iterator = engine.actors.begin();
		iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if (actor->get_block() && actor->get_x_pos() == x && actor->get_y_pos() == y) {
			return false; //if actor blocked and cannot walk
		}
	}
	return true;
}

bool Map::isInFov(int x, int y) const
{
	if (x < 0 || x >= this->width || y < 0 || y >= this->height) {
		return false;
	}
	if (this->map->isInFov(x, y)) {
		this->tiles[x + y * width].explored = true;
		return true;
	}
	return false;
}

bool Map::isExplored(int x, int y) const
{
	return this->tiles[x + y * width].explored;
}

void Map::addMonster(int x, int y)
{
	TCODRandom *rng = TCODRandom::getInstance();
	if (rng->getInt(0, 100) < 50) {
		Actor *orc = new Actor(x, y, 'o', "orc", TCODColor::desaturatedGreen);
		orc->destructible = new MonsterDestructible(10, 0, "dead orc", 15);
		orc->attacker = new Attacker(5);
		orc->ai = new MonsterAi();
		engine.actors.push(orc);
	} 
	else {
		Actor *troll = new Actor(x, y, 'T', "troll", TCODColor::darkerGreen);
		troll->destructible = new MonsterDestructible(16, 1, "troll carcass", 17);
		troll->attacker = new Attacker(4);
		troll->ai = new MonsterAi();
		engine.actors.push(troll);
	}
}

void Map::computeFov()
{
	this->map->computeFov(engine.player->get_x_pos(), engine.player->get_y_pos(), engine.get_fov_radius());
}

void Map::addItem(int x, int y)
{
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);
	if (dice < 40) {
		//create a health potion
		Actor *healthPotion = new Actor(x, y, '#', "scroll of heal", TCODColor::cyan);
		healthPotion->set_block(false);
		healthPotion->pickable = new Healer(4);
		engine.actors.push(healthPotion);
	}
	else if (dice < 40 + 15) {
		//create a scroll with lightning bolt
		Actor *scrollOfLightningBolt = new Actor(x, y, '#', "scroll of lightning bolt",
			TCODColor::lighterBlue);
		scrollOfLightningBolt->set_block(false);
		scrollOfLightningBolt->pickable = new LightningBolt(5, 20);
		engine.actors.push(scrollOfLightningBolt);
	}
	else if (dice < 40 + 15 + 15) {
		//create a scroll with fireball
		Actor *scrollOfFireball = new Actor(x, y, '#', "scroll of fireball",
			TCODColor::lightYellow);
		scrollOfFireball->set_block(false);
		scrollOfFireball->pickable = new Fireball(3, 15);
		engine.actors.push(scrollOfFireball);
	}
	else if (dice < 40 + 15 + 15 + 15) {
		//create a scroll with confusion
		Actor *scrollOfConfusion = new Actor(x, y, '#', "scroll of confusion",
			TCODColor::lightGrey);
		scrollOfConfusion->set_block(false);
		scrollOfConfusion->pickable = new Confuser(7, 8);
		engine.actors.push(scrollOfConfusion);
	}
	else {
		Actor *scrollOfLightningChain = new Actor(x, y, '#', "scroll of lightning chain",
			TCODColor::darkerBlue);
		scrollOfLightningChain->set_block(false);
		scrollOfLightningChain->pickable = new LightningChain(7, 15);
		engine.actors.push(scrollOfLightningChain);
	}
}

void Map::render() const
{
	static const TCODColor darkWall(0, 0, 100);
	static const TCODColor darkGround(50, 50, 150);
	static const TCODColor lightWall(130, 110, 50);
	static const TCODColor lightGround(200, 180, 50);

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			if (isInFov(x, y)) {
				TCODConsole::root->setCharBackground(x, y,
					this->isWall(x, y) ? lightWall : lightGround);
			}
			else if (isExplored(x, y)) {
				TCODConsole::root->setCharBackground(x, y,
					isWall(x, y) ? darkWall : darkGround);
			}
		}
}

void Map::init(bool withActors)
{
	rng = new TCODRandom(seed, TCOD_RNG_CMWC);
	this->tiles = new Tile[width*height];
	this->map = new TCODMap(width, height);
	TCODBsp bsp(0, 0, width, height);
	bsp.splitRecursive(nullptr, 16, ROOM_MAX_SIZE, ROOM_MAX_SIZE, 1.5f, 1.5f);
	BspListener listener(*this);
	bsp.traverseInvertedLevelOrder(&listener, (void*)withActors);
}

void Map::load(TCODZip & zip)
{
	seed = zip.getInt();
	init(false);
	for (int i = 0; i < width*height; i++) {
		tiles[i].explored = zip.getInt();
	}
}

void Map::save(TCODZip & zip)
{
	zip.putInt(seed);
	for (int i = 0; i < width*height; i++) {
		zip.putInt(tiles[i].explored);
	}
}

void Map::dig(int x1, int y1, int x2, int y2)
{
	if (x2 < x1) {
		int tmp = x2;
		x2 = x1;
		x1 = tmp;
	}
	if (y2 < y1) {
		int tmp = y2;
		y2 = y1;
		y1 = tmp;
	}

	for (int tilex = x1; tilex <= x2; tilex++) {
		for (int tiley = y1; tiley <= y2; tiley++) {
			this->map->setProperties(tilex, tiley, true, true);
		}
	}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors)
{
	this->dig(x1, y1, x2, y2);
	if (!withActors) {
		return;
	}

	if (first) {
		//player in first room
		engine.player->set_x_pos((x1 + x2) / 2);
		engine.player->set_y_pos((y1 + y2) / 2);
	}
	else {
		TCODRandom *rng = TCODRandom::getInstance();

		//add mosnters
		int nbMonsters = rng->getInt(1, MAX_ROOM_MONSTERS);
		while (nbMonsters > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y)) {
				addMonster(x, y);
			}
			nbMonsters--;
		}

		//add items
		int nbItems = rng->getInt(0, MAX_ROOM_ITEMS);
		while (nbItems > 0) {
			int x = rng->getInt(x1, x2);
			int y = rng->getInt(y1, y2);
			if (canWalk(x, y)) {
				addItem(x, y);
			}
			nbItems--;
		}
	}
	//set stairs position
	engine.stairs->set_x_pos((x1 + x2) / 2);
	engine.stairs->set_y_pos((y1 + y2) / 2);
}


