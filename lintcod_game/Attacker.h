#pragma once
class Attacker
{
	float power; //hp given
public:
	Attacker();
	Attacker(float power);
	~Attacker();
	void set_power(float power) { this->power = power; }
	float get_power() { return this->power; }
	void attack(Actor *owner, Actor *target);
};

