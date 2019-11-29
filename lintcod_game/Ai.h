#pragma once
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
	void update(Actor *owner) override;
	void handleActionKey(Actor *owner, int ascii);
	void load(TCODZip &zip);
	void save(TCODZip &zip);
protected:
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
	int moveCount=0;
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
