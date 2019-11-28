#pragma once
class Pickable
{
public:
	Pickable();
	virtual ~Pickable();
	bool pick(Actor *owner, Actor *wearer);
	virtual bool use(Actor *owner, Actor *wearer);
	void drop(Actor *owner, Actor *wearer);
};

class Healer : public Pickable {
private:
	float amount; //how many hp
public:
	Healer(float amount);
	bool use(Actor *owner, Actor *wearer) override;
	float get_amount() { return this->amount; }
	void set_amount(float amount) { this->amount = amount; }
};

class DamageSpell : public Pickable {
protected:
	float range, damage;
public:
	DamageSpell(float range, float damage);
	virtual bool use(Actor *owner, Actor *wearer) = 0;
};

class LightningBolt : public DamageSpell {
public:
	LightningBolt(float range, float damage);
	bool use(Actor *owner, Actor *wearer) override;
};

class Fireball : public DamageSpell {
public:
	Fireball(float range, float damage);
	bool use(Actor *owner, Actor *wearer) override;
};

class Confuser : public Pickable {
private:
	int nbTurns;
	float range;
public:
	Confuser(int nbTurns, float range);
	bool use(Actor *owner, Actor *wearer);
};