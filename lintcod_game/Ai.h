#pragma once
class Ai
{
public:
	Ai();
	virtual ~Ai();
	virtual void update(Actor* owner) = 0;
};

class PlayerAi : public Ai {
public:
	void update(Actor *owner) override;
	void handleActionKey(Actor *owner, int ascii);
protected:
	enum INVENTORY_STATUS {OPEN, CLOSE} inventory_status = CLOSE;
	bool moveOrAttack(Actor *owner, int targetx, int targety);
	Actor *choseFromInventory(Actor *owner);
};

class MonsterAi : public Ai {
public:
	void update(Actor *owner) override;
protected:
	int moveCount=0;
	void moveOrAttack(Actor *owner, int targetx, int targety);
};

class ConfusedMonsterAi : public Ai {
private:
	int nbTurns;
	Ai *oldAi;
public:
	ConfusedMonsterAi(int nbTurns, Ai *oldAi);
	void update(Actor *owner);
};
