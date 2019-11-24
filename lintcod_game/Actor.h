#pragma once
class Actor
{
	int x, y; //pos on map
	int ch; //symb ascii code
	TCODColor col; //color
	const char *name;//name
	 //can walk on this actor?
	
	
public:
	Attacker *attacker; // something deals damage
	Destructible *destructible; //something can be damaged
	Ai *ai; //something self-updating
	bool blocks;
	Actor();
	Actor(int, int, int, const char *name, const TCODColor&);
	virtual ~Actor();
	void update();
	void render() const;
	int get_x_pos() const;
	int get_y_pos() const;
	void set_x_pos(int);
	void set_y_pos(int);
	void set_ch(int ch) ;
	int get_ch();
	void set_color(TCODColor col);
	TCODColor get_color();
	void set_name(const char* name);
	const char* get_name();
};

