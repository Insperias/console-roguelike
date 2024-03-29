#pragma once
// after 20 turns, the monster cannot smell the scent anymore
static const int SCENT_THRESHOLD = 20;

class Ai : public Persistent
{
public:
	Ai();
	virtual ~Ai();
	virtual void update(Actor* owner) = 0;
	static Ai *create(TCODZip &zip);
protected:
	enum AiType {MONSTER, CONFUSED_MONSTER, PLAYER};
};

class PlayerAi : public Ai {
public:
	PlayerAi();
	void update(Actor *owner) override;
	void handleActionKey(Actor *owner, int ascii);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
	int getNextLevelXp();

	//accessors
	int get_xp_level() const { return this->xpLevel; }
protected:
	int xpLevel;
	enum INVENTORY_STATUS {OPEN, CLOSE} inventory_status = CLOSE;
	bool moveOrAttack(Actor *owner, int targetx, int targety);
	Actor *choseFromInventory(Actor *owner);
};

class MonsterAi : public Ai {
public:
	void update(Actor *owner) override;
	void load(TCODZip &zip);
	void save(TCODZip &zip);
protected:
	void moveOrAttack(Actor *owner, int targetx, int targety);
};

class ConfusedMonsterAi : public Ai {
protected:
	int nbTurns;
	Ai *oldAi;
public:
	ConfusedMonsterAi(int nbTurns, Ai *oldAi);
	void update(Actor *owner);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
};
