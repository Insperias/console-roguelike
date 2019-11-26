#pragma once
class Destructible
{
	float maxHp;//max hp
	float hp;//current hp
	float defense;// deflected hp
	const char *corpseName;// the actor's name once dead/destroyed
public:
	Destructible();
	Destructible(float maxHp, float defense, const char *corpseName);
	inline bool isDead() { return hp <= 0; }
	virtual ~Destructible();
	float takeDamage(Actor* owner, float damage);
	virtual void die(Actor *owner);
	float heal(float amount);
	void set_defense(float defense) { this->defense = defense; }
	float get_defense() { return this->defense; }
	void set_hp(float hp) { this->hp = hp; }
	float get_hp() { return this->hp; }
	float get_maxHp() { return this->maxHp; }
	void set_maxHp(float maxHp) { this->maxHp = maxHp; }
};

class MonsterDestructible : public Destructible {
public:
	MonsterDestructible(float maxHp, float defense, const char *corpseName);
	void die(Actor *owner) override;
};

class PlayerDestructible : public Destructible {
public:
	PlayerDestructible(float maxHp, float defense, const char *corpseName);
	void die(Actor *owner) override;
};
