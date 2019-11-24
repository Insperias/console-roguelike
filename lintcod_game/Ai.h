#pragma once
class Ai
{
public:
	Ai();
	~Ai();
	virtual void update(Actor* owner) = 0;
};

class PlayerAi : public Ai {
public:
	void update(Actor *owner) override;
protected:
	bool moveOrAttack(Actor *owner, int targetx, int targety);
};

class MonsterAi : public Ai {
public:
	void update(Actor *owner) override;
protected:
	int moveCount=0;
	void moveOrAttack(Actor *owner, int targetx, int targety);
};
