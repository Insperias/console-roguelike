#pragma once
class Pickable : public Persistent
{
public:
	Pickable();
	virtual ~Pickable();
	bool pick(Actor *owner, Actor *wearer);
	virtual bool use(Actor *owner, Actor *wearer);
	void drop(Actor *owner, Actor *wearer);
	static Pickable *create(TCODZip &zip);
protected:
	enum PickableType {HEALER, LIGHTNING_BOLT, CONFUSER, FIREBALL, LIGHTNING_CHAIN};
};

class Healer : public Pickable {
private:
	float amount; //how many hp
public:
	Healer(float amount);
	bool use(Actor *owner, Actor *wearer) override;
	float get_amount() { return this->amount; }
	void set_amount(float amount) { this->amount = amount; }
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};

class DamageSpell : public Pickable {
protected:
	float range, damage;
public:
	DamageSpell(float range, float damage);
	void load(TCODZip &zip);
};

class DebuffSpell : public Pickable {
protected:
	int nbTurns;
	float range;
public:
	DebuffSpell(int nbTurns, float range);
	void load(TCODZip &zip);

};

class LightningBolt : public DamageSpell {
public:
	LightningBolt(float range, float damage);
	bool use(Actor *owner, Actor *wearer) override;
	void save(TCODZip &zip);
};

class LightningChain : public DamageSpell {
public:
	LightningChain(float range, float damage);
	bool use(Actor* owner, Actor *wearer) override;
	void save(TCODZip &zip);
};

class Fireball : public DamageSpell {
public:
	Fireball(float range, float damage);
	bool use(Actor *owner, Actor *wearer) override;
	void save(TCODZip &zip);
};

class Confuser : public DebuffSpell {
public:
	Confuser(int nbTurns, float range);
	bool use(Actor *owner, Actor *wearer);
	void save(TCODZip &zip);
};