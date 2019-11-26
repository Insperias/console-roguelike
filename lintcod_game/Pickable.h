#pragma once
class Pickable
{
public:
	Pickable();
	virtual ~Pickable();
	bool pick(Actor *owner, Actor *wearer);
	virtual bool use(Actor *owner, Actor *wearer);
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