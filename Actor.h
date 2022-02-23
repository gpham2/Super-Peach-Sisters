#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"


// ACTOR HEADER
class StudentWorld;
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* world, int imgID, int x, int y, int dir, int depth);
	virtual ~Actor() {};

	// virtuals
	virtual void doSomething() = 0;
	virtual void bonk(Actor* bonker) {};
	virtual void damaged() {};
	virtual Actor* create() { return this; }

	virtual bool isBlocker() const { return false; }
	virtual bool isDamageable() const { return false; }

	// assessors
	bool isAlive() const { return m_alive; }
	StudentWorld* world() const { return m_world; }

	// mutators
	void die() { m_alive = false; }
	void decideTurn(int dist, bool turnAtLedge);
	bool tryMove(int dir, int dist, bool bonkBlocker);

private:
	bool m_alive;
	StudentWorld* m_world;
};


// BLOCKER HEADER
class Blocker : public Actor
{
public:
	Blocker(StudentWorld* world, int imgID, int x, int y);
	virtual ~Blocker() {};

	// virtuals
	virtual void doSomething() = 0;
	virtual bool isBlocker() const { return true; }

	// accessors

	// mutators

private:
};


// CREATURE HEADER
class Creature : public Actor
{
public:
	Creature(StudentWorld* world, int imgID, int x, int y, int dir);
	virtual ~Creature() {};

	// virtuals
	virtual void doSomething() = 0;
	virtual bool isDamageable() const { return true; }

	// accessors

	// mutators
	bool aliveUpdateBonk();

private:
	// private functions
	virtual bool doDifferentStuff();
};

// ITEM HEADER
class Item : public Actor
{
public:
	Item(StudentWorld* world, int imgID, int x, int y, int dir);
	virtual ~Item() {};

	// mutators
	void powerUpMove(int powerType);
	void projectileMove(bool hurtEnemy);
	void goalDetect(bool endGame);
	void fallDown();

};


// PEACH HEADER
class Peach : public Creature
{
public:
	Peach(StudentWorld* world, int x, int y, int dir, int depth);
	virtual ~Peach() {};

	// virtuals
	virtual void doSomething();
	virtual void bonk(Actor* bonker);

	// accessors
	bool hasStar() { return m_starPower; }
	bool hasShoot() { return m_shootPower; }
	bool hasJump() { return m_jumpPower; }

	// mutators
	void getStar() { m_starPower = true; m_invincible = 150; }
	void getShoot() { m_shootPower = true; }
	void getJump() { m_jumpPower = true; }
	void setHp() { m_hp = 2; }


private:
	// private functions
	virtual bool doDifferentStuff();

	int m_hp;
	int m_invincible;
	int m_recharge;
	int m_jumpLeft;
	bool m_jumping;
	bool m_starPower;
	bool m_shootPower;
	bool m_jumpPower;
};


// BLOCK HEADER
class Block : public Blocker
{
public:
	Block(StudentWorld* world, int x, int y, int goodie);
	virtual ~Block() {};

	// virtuals
	virtual void doSomething() {};
	virtual void bonk(Actor* bonker);

private:
	int m_goodie;
};


// PIPE HEADER
class Pipe : public Blocker
{
public:
	Pipe(StudentWorld* world, int x, int y);
	virtual ~Pipe() {};

	// virtuals
	virtual void doSomething() {}
	virtual void bonk(Actor* bonker, bool starPower);

private:
};


// GOOMBA HEADER
class Goomba : public Creature
{
public:
	Goomba(StudentWorld* world, int x, int y, int dir);
	virtual ~Goomba() {};

	// virtuals
	virtual void doSomething();
	virtual void bonk(Actor* bonker);
	virtual void damaged();

private:

};


// KOOPER HEADER
class Koopa : public Creature
{
public:
	Koopa(StudentWorld* world, int x, int y, int dir);
	virtual ~Koopa() {};

	// virtuals
	virtual void doSomething();
	virtual void bonk(Actor* bonker);
	virtual void damaged();

private:

};


// PIRANHA HEADER
class Piranha : public Creature
{
public:
	Piranha(StudentWorld* world, int x, int y, int dir);
	virtual ~Piranha() {};

	// virtuals
	virtual void doSomething();
	virtual void bonk(Actor* bonker);
	virtual void damaged();

private:
	virtual bool doDifferentStuff();
	int m_firingDelay;
};

// MUSHROOM HEADER
class Mushroom : public Item
{
public:
	Mushroom(StudentWorld* world, int x, int y);
	virtual ~Mushroom() {};

	// virtuals
	virtual void doSomething() { powerUpMove(3); }
	virtual Actor* create() { return new Mushroom(world(), (int)getX(), (int)getY()); }

private:
};


// FLOWER HEADER
class Flower : public Item
{
public:
	Flower(StudentWorld* world, int x, int y);
	virtual ~Flower() {};

	// virtuals
	virtual void doSomething() { powerUpMove(2); }
	virtual Actor* create() { return new Flower(world(), (int)getX(), (int)getY()); }

private:
};


// STAR HEADER
class Star : public Item
{
public:
	Star(StudentWorld* world, int x, int y);
	virtual ~Star() {};

	// virtuals
	virtual void doSomething() { powerUpMove(1); }
	virtual Actor* create() { return new Star(world(), (int)getX(), (int)getY()); }

private:
};


// SHELL HEADER
class Shell : public Item
{
public:
	Shell(StudentWorld* world, int x, int y, int dir);
	virtual ~Shell() {};

	// virtuals
	virtual void doSomething();
	virtual Actor* create() { return new Shell(world(), (int)getX(), (int)getY(), getDirection()); }

private:

};


// PEACHFIREBALL HEADER
class PeachFireBall : public Item
{
public:
	PeachFireBall(StudentWorld* world, int x, int y, int dir);
	virtual ~PeachFireBall() {};

	// virtuals
	virtual void doSomething();
	virtual Actor* create() { return new PeachFireBall(world(), (int)getX(), (int)getY(), getDirection()); }

private:

};


// PEACHFIREBALL HEADER
class PiranhaFireBall : public Item
{
public:
	PiranhaFireBall(StudentWorld* world, int x, int y, int dir);
	virtual ~PiranhaFireBall() {};

	// virtuals
	virtual void doSomething();
	virtual Actor* create() { return new PiranhaFireBall(world(), (int)getX(), (int)getY(), getDirection()); }

private:

};

// GOALS HEADER
class Flag : public Item
{
public:
	Flag(StudentWorld* world, int x, int y);
	virtual ~Flag() {};

	virtual void doSomething();

private:
};

class Mario : public Item
{
public:
	Mario(StudentWorld* world, int x, int y);
	virtual ~Mario() {};

	virtual void doSomething();

private:
};


#endif // ACTOR_H_
