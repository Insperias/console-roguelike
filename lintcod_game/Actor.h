#pragma once
class Actor : public Persistent
{
	int x, y; //pos on map
	int ch; //symb ascii code
	TCODColor col; //color
	const char *name;//name
	bool blocks;//can walk on this actor?
	bool fovOnly; // display when in fov
	
	
public:
	Attacker *attacker; // something deals damage
	Destructible *destructible; //something can be damaged
	Ai *ai; //something self-updating
	Pickable *pickable; //something can be picked and used
	Container *container; //something that can contain actor
	
	Actor();
	Actor(int, int, int, const char *name, const TCODColor&);
	virtual ~Actor();
	void update();
	void render() const;
	float getDistance(int cx, int cy) const;
	void load(TCODZip &zip);
	void save(TCODZip &zip);

	//accessors
	int get_x_pos() const;
	int get_y_pos() const;
	void set_x_pos(int);
	void set_y_pos(int);
	void set_ch(int ch) ;
	int get_ch() const;
	void set_color(TCODColor col);
	TCODColor get_color() const;
	void set_name(const char* name);
	const char* get_name() const;
	void set_block(bool block) { this->blocks = block; }
	bool get_block() const { return this->blocks; }
	void set_fov_only(bool fovOnly) { this->fovOnly = fovOnly; }
	bool get_fov_only() const { return this->fovOnly; }
};

