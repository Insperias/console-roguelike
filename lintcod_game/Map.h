#pragma once

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;


struct Tile {
	bool explored; //player seen tile?
	Tile() :explored(false) {}
}; //Tile data



class Map
{
private:
	int width, height;
	Tile *tiles; //tiles on map
	TCODMap *map;
	friend class BspListener;

	void dig(int x1, int y1, int x2, int y2);
	void createRoom(bool first, int x1, int y1, int x2, int y2);
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
	void render() const;

	//accessors
	int get_width() const { return this->width; }
	void set_width(int width) { this->width = width; }
	int get_height() const { return this->height; }
	void set_height(int height) { this->height = height; }

protected:
	

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
			//dig a room
			TCODRandom *rng = TCODRandom::getInstance();
			w = rng->getInt(ROOM_MIN_SIZE, node->w - 2);
			h = rng->getInt(ROOM_MIN_SIZE, node->h - 2);
			x = rng->getInt(node->x + 1, node->x + node->w - w - 1);
			y = rng->getInt(node->y + 1, node->y + node->h - h - 1);
			map.createRoom(roomNum == 0, x, y, x + w - 1, y + h - 1);

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