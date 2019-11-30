#pragma once

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;


struct Tile {
	bool explored; //player seen tile?
	unsigned int scent; //amount fo player scent on this cell
	Tile() :explored(false), scent(0) {}
}; //Tile data



class Map : Persistent
{
private:
	int width, height;
	unsigned int currentScentValue;
	Tile *tiles; //tiles on map
	TCODMap *map;
	long seed;
	TCODRandom *rng;
	friend class BspListener;

	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2, bool withActors);
public:
	Map();
	Map(int, int);
	~Map();
	bool isWall(int, int) const;
	bool canWalk(int x, int y) const;
	bool isInFov(int x, int y) const;
	bool isExplored(int x, int y) const; 
	void addMonster(int x, int y);
	void computeFov(); // player field of vision
	void addItem(int x, int y);
	unsigned int getScent(int x, int y) const;
	void render() const;
	void init(bool withActors);
	void load(TCODZip &zip);
	void save(TCODZip &zip);

	//accessors
	int get_width() const { return this->width; }
	void set_width(int width) { this->width = width; }
	int get_height() const { return this->height; }
	void set_height(int height) { this->height = height; }
	void set_current_scent(unsigned int scent) { this->currentScentValue = scent; }
	unsigned get_current_scent() const { return this->currentScentValue; }
};

class BspListener : public ITCODBspCallback { //using bsp-algorithm for random map creating
	Map &map; //map to dig
	int roomNum; //room number
	int lastx, lasty; //last room's center
public:
	BspListener(Map & map) :map(map), roomNum(0) {}
	bool visitNode(TCODBsp *node, void* userData) {
		if (node->isLeaf()) {
			int x, y, w, h;
			bool withActors = (bool)userData;
			//dig a room
			w = map.rng->getInt(ROOM_MIN_SIZE, node->w - 2);
			h = map.rng->getInt(ROOM_MIN_SIZE, node->h - 2);
			x = map.rng->getInt(node->x + 1, node->x + node->w - w - 1);
			y = map.rng->getInt(node->y + 1, node->y + node->h - h - 1);
			map.createRoom(roomNum == 0, x, y, x + w - 1, y + h - 1, withActors);

			if (roomNum != 0) {
				//dig a corridor from last room
				map.dig(lastx, lasty, x + w / 2, lasty);
				map.dig(x + w / 2, lasty, x + w / 2, y + h / 2);
			}
			lastx = x + w / 2;
			lasty = y + h / 2;
			roomNum++;
		}
		return true;
	}
};