#pragma once
class Pickable : public Persistent
{
public:
	enum SpellType {DAMAGE_SPELL, DEBUFF_SPELL, HEAL_SPELL};
	Pickable(SpellType type);
	virtual ~Pickable();
	bool pick(Actor *owner, Actor *wearer);
	virtual bool use(Actor *owner, Actor *wearer);
	void drop(Actor *owner, Actor *wearer);
	static Pickable *create(TCODZip &zip);
	
	//accessors
	SpellType get_type() const { return this->type; }
protected:
	enum PickableType {HEALER, LIGHTNING_BOLT, CONFUSER, FIREBALL, LIGHTNING_CHAIN};
	SpellType type;
};

class Healer : public Pickable {
private:
	float amount; //how many hp
public:
	Healer(float amount);
	bool use(Actor *owner, Actor *wearer) override;
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	
	//accessors
	void set_amount(float amount) { this->amount = amount; }
	float get_amount() const { return this->amount; }
};

class DamageSpell : public Pickable {
protected:
	float range, damage;
public:
	DamageSpell(float range, float damage);
	void load(TCODZip &zip);
	
	//accesors
	void set_damage(float damage) { this->damage = damage; }
	float get_damage() const { return this->damage; }
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