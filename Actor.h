#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* world, int imgID, int x, int y, int dir, int depth);
	~Actor() {}

	// virtuals
	virtual void doSomething() = 0;
	virtual void bonk(Actor* bonker, bool starPower) {};
	virtual void damage() {};
	virtual Actor* create() { return this; }

	virtual bool isBlocker() const { return false; }
	virtual bool isDamageable() const { return false; }
	virtual bool isPeach() const { return false; }
	

	// assessors
	bool isAlive() const { return m_alive; }
	StudentWorld* world() const { return m_world; }

	// mutators
	void die() { m_alive = false; }

	// repeatables
	void powerDo(int powerType);

private:
	bool m_alive;
	StudentWorld* m_world;
};

class Blocker : public Actor
{
public:
	Blocker(StudentWorld* world, int imgID, int x, int y, int dir, int depth);
	~Blocker() {}

	// virtuals
	virtual void doSomething() = 0;
	virtual void bonk(Actor* other, bool starPower) {}
	virtual bool isBlocker() const { return true; }

	// accessors

	// mutators

private:
};

class Creature : public Actor
{
public:
	Creature(StudentWorld* world, int imgID, int x, int y, int dir, int depth);
	~Creature() {}

	// virtuals
	virtual void doSomething() = 0;
	virtual void bonk(Actor* other, bool starPower) {};
	virtual bool isDamageable() const { return true; }

	// accessors

	// mutators

private:
};


class Peach : public Creature
{
public:
	Peach(StudentWorld* world, int x, int y, int dir, int depth);
	~Peach() {}

	// virtuals
	virtual void doSomething();
	virtual void bonk(Actor* bonker, bool starPower);
	virtual bool isPeach() const { return true; }

	// accessors
	bool hasStar() { return m_starPower; }
	bool hasShoot() { return m_shootPower; }
	bool hasJump() { return m_jumpPower; }

	// mutators
	void getStar() { m_starPower = true; m_invincible = 150; }
	void getShoot() { m_shootPower = true; }
	void getJump() { m_jumpPower = true; }
	void incHp() { m_hp = 2; }

private:
	int m_hp;
	int m_invincible;
	int m_recharge;
	int m_jumpLeft;
	bool m_jumping;
	bool m_starPower;
	bool m_shootPower;
	bool m_jumpPower;
};

class Piranha : public Creature
{
public:
	Piranha(StudentWorld* world, int x, int y, int dir);
	~Piranha() {}

	// virtuals
	virtual void doSomething();
	virtual void bonk(Actor* bonker, bool starPower);
	virtual void damage();

private:
	int m_firingDelay;
};


class Goomba : public Creature
{
public:
	Goomba(StudentWorld* world, int x, int y, int dir);
	~Goomba() {}

	// virtuals
	virtual void doSomething();
	virtual void bonk(Actor* bonker, bool starPower);
	virtual void damage();

private:

};

class Koopa : public Creature
{
public:
	Koopa(StudentWorld* world, int x, int y, int dir);
	~Koopa() {}

	// virtuals
	virtual void doSomething();
	virtual void bonk(Actor* bonker, bool starPower);
	virtual void damage();

private:

};

class Shell : public Actor
{
public:
	Shell(StudentWorld* world, int x, int y, int dir);
	~Shell() {}

	// virtuals
	virtual void doSomething();
	virtual Actor* create() { return new Shell(world(), getX(), getY(), getDirection()); }

private:

};




class Block : public Blocker
{
public:
	Block(StudentWorld* world, int x, int y, int goodie);
	~Block() {}

	// virtuals
	virtual void doSomething() {}
	virtual void bonk(Actor* bonker, bool starPower);

private:
	int m_goodie;
};


class Pipe : public Blocker
{
public:
	Pipe(StudentWorld* world, int x, int y);
	~Pipe() {}

	// virtuals
	virtual void doSomething() {}
	virtual void bonk(Actor* bonker, bool starPower);

private:
};





class Mushroom : public Actor
{
public:
	Mushroom(StudentWorld* world, int x, int y);
	~Mushroom() {}

	// virtuals
	virtual void doSomething();
	virtual Actor* create() { return new Mushroom(world(), getX(), getY()); }

private:
};

class Flower : public Actor
{
public:
	Flower(StudentWorld* world, int x, int y);
	~Flower() {}

	// virtuals
	virtual void doSomething();
	virtual Actor* create() { return new Flower(world(), getX(), getY()); }


};

class Star : public Actor
{
public:
	Star(StudentWorld* world, int x, int y);
	~Star() {}

	// virtuals
	virtual void doSomething();
	virtual Actor* create() { return new Star(world(), getX(), getY()); }


};

class PeachFireBall : public Actor
{
public:
	PeachFireBall(StudentWorld* world, int x, int y, int dir);
	~PeachFireBall() {}

	// virtuals
	virtual void doSomething();
	virtual Actor* create() { return new PeachFireBall(world(), getX(), getY(), getDirection()); }

private:

};

class PiranhaFireBall : public Actor
{
public:
	PiranhaFireBall(StudentWorld* world, int x, int y, int dir);
	~PiranhaFireBall() {}

	// virtuals
	virtual void doSomething();
	virtual Actor* create() { return new PiranhaFireBall(world(), getX(), getY(), getDirection()); }

private:

};

class Flag : public Actor
{
public:
	Flag(StudentWorld* world, int x, int y, int dir);
	~Flag() {}

	virtual void doSomething();
private:
};

class Mario : public Actor
{
public:
	Mario(StudentWorld* world, int x, int y, int dir);
	~Mario() {}

	virtual void doSomething();
private:
};





#endif // ACTOR_H_
