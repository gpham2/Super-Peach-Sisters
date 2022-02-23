#include "Actor.h"
#include "StudentWorld.h"

// ACTOR CLASS

Actor::Actor(StudentWorld* world, int imgID, int x, int y, int dir, int depth)
	: GraphObject(imgID, x, y, dir, depth), m_alive(true), m_world(world) {}

void Actor::decideTurn(int dist, bool turnAtLedge)
{
	int blockX = (int)getX() + (90 - getDirection()) / 90 * dist;
	int fallX = (int)getX() + (90 - getDirection()) / 90 * (dist + SPRITE_WIDTH - 1);
	bool blocked = world()->isBlockingObjectAt(blockX, blockX + SPRITE_WIDTH - 1, (int)getY(), (int)getY() + SPRITE_HEIGHT - 1, false);
	bool falloff = !world()->isBlockingObjectAt(fallX, fallX + SPRITE_WIDTH - 1, getY() - SPRITE_HEIGHT, getY() - 1, false);
	if (blocked || (turnAtLedge && falloff)) setDirection((180 - getDirection()));
}

bool Actor::tryMove(int dir, int dist, bool bonkBlock)
{
	// tries move in selected direction, return true if can, return false if can't
	setDirection(dir);
	int newX = (int)getX() + (90-dir)/90 * dist;
	if (!world()->isBlockingObjectAt((int)newX, (int)newX + SPRITE_WIDTH - 1, (int)getY(), (int)getY() + SPRITE_HEIGHT - 1, bonkBlock))
	{
		moveTo(newX, getY());
		return true;
	}
	return false;
		
}

// BLOCKER CLASS
Blocker::Blocker(StudentWorld* world, int imgID, int x, int y)
	: Actor(world, imgID, x, y, 0, 2) {}


// CREATURE CLASS
Creature::Creature(StudentWorld* world, int imgID, int x, int y, int dir)
	: Actor(world, imgID, x, y, dir, 0) {}

bool Creature::aliveUpdateBonk()
{
	// returns true to indicate function calling it should exit
	if (isAlive() == false) return true;
	return doDifferentStuff();
}

bool Creature::doDifferentStuff()
{
	return(world()->bonkPeach(this));
}


// ITEM CLASS
Item::Item(StudentWorld* world, int imgID, int x, int y, int dir)
	: Actor(world, imgID, x, y, dir, 1) {}

void Item::powerUpMove(int powerType)
{
	// 1:star, 2:flower, 3:mushroom
	if (world()->isPeachHere(this, false))
	{
		switch (powerType)
		{
		case 1: world()->peachGetStar(); break;
		case 2: world()->peachGetShoot(); break;
		case 3: world()->peachGetJump(); break;
		default: return;
		}
		world()->playSound(SOUND_PLAYER_POWERUP);
		die();
		return;
	}
	fallDown();
	decideTurn(2, false);
	tryMove(getDirection(), 2, false);
}

void Item::projectileMove(bool hurtEnemy)
{

	if (hurtEnemy && world()->bonkAllNonBlocks(this, true))
	{
		die();
		return;
	}
	else if (!hurtEnemy && world()->isPeachHere(this, true))
	{
		die();
		return;
	}
	fallDown();
	if (tryMove(getDirection(), 2, false) == false) die();
}

void Item::goalDetect(bool endGame)
{
	if (isAlive() == false) return;
	if (world()->isPeachHere(this, false))
	{
		world()->increaseScore(1000);
		if (endGame) world()->win();
		else world()->nextLevel();
		die();
		return;
	}
}


void Item::fallDown()
{
	if (world()->isBlockingObjectAt(getX(), getX() + SPRITE_WIDTH - 1, getY() - 1, getY(), false) == false)
		moveTo(getX(), getY() - 2);
}



// PEACH CLASS
Peach::Peach(StudentWorld* world, int x, int y, int dir, int depth)
	: Creature(world, IID_PEACH, x, y, 0),
	m_hp(1), m_invincible(0), m_recharge(0), m_jumpLeft(0), m_jumping(0), m_starPower(0), m_shootPower(0), m_jumpPower(0) {}

void Peach::doSomething()
{
	// check alive, update timers, bonk all overlapped
	if (aliveUpdateBonk()) return;

	// check if jumping
	if (m_jumping && m_jumpLeft > 0)
	{
		int newY = (int)getY() + 4;
		if (world()->isBlockingObjectAt((int)getX(), (int)getX() + SPRITE_WIDTH - 1, newY, newY + SPRITE_HEIGHT - 1, true))
		{
			m_jumpLeft = 0;
			m_jumping = false;
		}
		else
		{
			moveTo(getX(), newY);
			m_jumpLeft--;
		}
	}

	// check if falling
	else
	{
		if (world()->isBlockingObjectAt((int)getX(), (int)getX() + SPRITE_WIDTH - 1, (int)getY() - 3, (int)getY(), false) == false)
			moveTo(getX(), getY() - 4);
	}

	// check if user pressed key
	int keyPressed;
	if (world()->getKey(keyPressed))
	{
		switch (keyPressed)
		{
		case (KEY_PRESS_LEFT): tryMove(180, 4, true); break;
		case (KEY_PRESS_RIGHT): tryMove(0, 4, true); break;
		case (KEY_PRESS_UP):
			if (world()->isBlockingObjectAt(getX(), getX() + SPRITE_WIDTH - 1, getY() - 1, getY(), false))
			{
				m_jumpLeft = m_jumpPower == true ? 12 : 8;
				world()->playSound(SOUND_PLAYER_JUMP);
				m_jumping = true;
			} break;
		case (KEY_PRESS_SPACE):
			if (m_shootPower && m_recharge == 0)
			{
				world()->playSound(SOUND_PLAYER_FIRE);
				m_recharge = 8;
				int newX = getDirection() == 0 ? (int)getX() + 4 : (int)getX() - 4;
				world()->createActor(new PeachFireBall(world(), newX, getY(), getDirection()));
			} break;
		}
	}
}

void Peach::bonk(Actor* bonker)
{
	// loses hp and powers
	if (m_invincible == 0)
	{
		m_hp--;
		m_invincible = 10;
		m_shootPower = false;
		m_jumpPower = false;
		if (m_hp > 0) world()->playSound(SOUND_PLAYER_HURT);
		else die();
	}
}

bool Peach::doDifferentStuff()
{
	if (m_starPower)
	{
		m_invincible--;
		if (m_invincible == 0) m_starPower = false;
	}
	else if (m_invincible > 0) m_invincible--;
	if (m_recharge > 0) m_recharge--;
	world()->bonkAllNonBlocks(this, false);
	return false;
}


// BLOCK CLASS
Block::Block(StudentWorld* world, int x, int y, int goodie)
	: Blocker(world, IID_BLOCK, x, y), m_goodie(goodie) {}

void Block::bonk(Actor* bonker)
{
	if (m_goodie != 0)
	{
		world()->playSound(SOUND_POWERUP_APPEARS);
		Actor* powerUp = nullptr;
		switch (m_goodie)
		{
		case 1: powerUp = new Star(world(), getX(), getY() + 8); break;
		case 2: powerUp = new Flower(world(), getX(), getY() + 8); break;
		case 3: powerUp = new Mushroom(world(), getX(), getY() + 8); break;
		}
		world()->createActor(powerUp);
		m_goodie = 0;
	}
	else world()->playSound(SOUND_PLAYER_BONK);
}


// PIPE CLASS
Pipe::Pipe(StudentWorld* world, int x, int y)
	: Blocker(world, IID_PIPE, x, y) {}

void Pipe::bonk(Actor* bonker, bool starPower)
{
	world()->playSound(SOUND_PLAYER_BONK);
}


// GOOMBA CLASS
Goomba::Goomba(StudentWorld* world, int x, int y, int dir)
	: Creature(world, IID_GOOMBA, x, y, dir) {}

void Goomba::doSomething()
{
	if (aliveUpdateBonk()) return;		 // check alive and bonks peach if overlap
	decideTurn(1, true);				 // decide if need to turn
	tryMove(getDirection(), 1, false);   // move in current direction
}

void Goomba::bonk(Actor* bonker)
{
	if (world()->peachCanKill(bonker))
	{
		world()->playSound(SOUND_PLAYER_KICK);
		damaged();
	}
}

void Goomba::damaged()
{
	world()->increaseScore(100);
	die();
}


// KOOPA CLASS
Koopa::Koopa(StudentWorld* world, int x, int y, int dir)
	: Creature(world, IID_KOOPA, x, y, dir) {}

void Koopa::doSomething()
{
	if (aliveUpdateBonk()) return;		 // check alive and bonks peach if overlap
	decideTurn(1, true);				 // decide if need to turn
	tryMove(getDirection(), 1, false);   // move in current direction
}

void Koopa::bonk(Actor* bonker)
{
	if (world()->peachCanKill(bonker))
	{
		world()->playSound(SOUND_PLAYER_KICK);
		damaged();
	}
}

void Koopa::damaged()
{
	world()->increaseScore(100);
	world()->createActor(new Shell(world(), getX(), getY(), getDirection()));
	die();
}


// PIRANHA CLASS
Piranha::Piranha(StudentWorld* world, int x, int y, int dir)
	: Creature(world, IID_PIRANHA, x, y, dir), m_firingDelay(0) {}

void Piranha::doSomething()
{
	// check alive, update animation, tries bonk peach
	if (aliveUpdateBonk()) return;
	if (world()->peachInPiranhaRange(this) == false) return;

	// faces and find x coordinate of peach
	int peachX = 0;
	world()->piranhaFacePeach(this, peachX);

	// update firing timer
	if (m_firingDelay > 0)
	{
		m_firingDelay--;
		return;
	}

	// if peach in range then fire
	if (getX() - 8 * SPRITE_WIDTH < peachX && peachX < getX() + 8 * SPRITE_WIDTH)
	{
		world()->createActor(new PiranhaFireBall(world(), getX(), getY(), getDirection()));
		world()->playSound(SOUND_PIRANHA_FIRE);
		m_firingDelay = 40;
	}
}

void Piranha::bonk(Actor* bonker)
{
	if (world()->peachCanKill(bonker))
	{
		world()->playSound(SOUND_PLAYER_KICK);
		damaged();
	}
}

void Piranha::damaged()
{
	world()->increaseScore(100);
	die();
}

bool Piranha::doDifferentStuff()
{
	increaseAnimationNumber();
	return (world()->bonkPeach(this));
}



// POWERUPS CLASSES
Mushroom::Mushroom(StudentWorld* world, int x, int y)
	: Item(world, IID_MUSHROOM, x, y, 0) {}

Flower::Flower(StudentWorld* world, int x, int y)
	: Item(world, IID_FLOWER, x, y, 0) {}

Star::Star(StudentWorld* world, int x, int y)
	: Item(world, IID_STAR, x, y, 0) {}


// PROJETILE CLASSES
Shell::Shell(StudentWorld* world, int x, int y, int dir)
	: Item(world, IID_SHELL, x, y, dir) {}

void Shell::doSomething()
{
	projectileMove(true);
}

PeachFireBall::PeachFireBall(StudentWorld* world, int x, int y, int dir)
	: Item(world, IID_PEACH_FIRE, x, y, dir) {}

void PeachFireBall::doSomething()
{
	projectileMove(true);
}

PiranhaFireBall::PiranhaFireBall(StudentWorld* world, int x, int y, int dir)
	: Item(world, IID_PIRANHA_FIRE, x, y, dir) {}

void PiranhaFireBall::doSomething()
{
	projectileMove(false);
}


// GOALS CLASSES
Mario::Mario(StudentWorld* world, int x, int y)
	: Item(world, IID_MARIO, x, y, 0) {}

void Mario::doSomething()
{
	goalDetect(true);
}

Flag::Flag(StudentWorld* world, int x, int y)
	: Item(world, IID_FLAG, x, y, 0) {}

void Flag::doSomething()
{
	goalDetect(false);
}