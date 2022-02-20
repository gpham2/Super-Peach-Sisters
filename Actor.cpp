#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


// CONSTRUCTORS

Actor::Actor(StudentWorld* world, int imgID, int x, int y, int dir, int depth)
	: GraphObject(imgID, x, y, dir, depth), m_alive(true), m_world(world) {}

Blocker::Blocker(StudentWorld* world, int imgID, int x, int y, int dir, int depth)
	: Actor(world, imgID, x, y, dir, depth) {}

Creature::Creature(StudentWorld* world, int imgID, int x, int y, int dir, int depth)
	: Actor(world, imgID, x, y, dir, depth) {}


// REPEATING FUNCTIONS

void Actor::powerDo(int powerType)
{
	// 1:star, 2:flower, 3:mushroom
	Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isPeach())
	{
		switch (powerType)
		{
		case 1: world()->increaseScore(100); world()->getPeach()->getStar(); break;
		case 2: world()->increaseScore(50); world()->getPeach()->getShoot(); world()->getPeach()->incHp(); break;
		case 3: world()->increaseScore(75); world()->getPeach()->getJump(); world()->getPeach()->incHp(); break;
		default: return;
		}
		world()->playSound(SOUND_PLAYER_POWERUP);
		die();
		return;
	}
	overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY() - 1, getY(), this, true);
	if (overlap->isBlocker() == false) moveTo(getX(), getY() - 2);
	int newX = (int)getX() + (90 - getDirection()) / 90 * 2;
	overlap = world()->overlap(newX, newX + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isBlocker())
	{
		setDirection(180 - getDirection());
		return;
	}
	moveTo(newX, getY());
}



// PIRANHA IMPLEMENTATION
Piranha::Piranha(StudentWorld* world, int x, int y, int dir)
	: Creature(world, IID_PIRANHA, x, y, dir, 0), m_firingDelay(0) {}

void Piranha::doSomething()
{
	// 1
	if (isAlive() == false) return;

	// 2
	increaseAnimationNumber();

	// 3
	Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isPeach())
	{
		overlap->bonk(this, false);
		return;
	}

	// 4 & 5
	int peachY = (int)world()->getPeach()->getY();
	if (getY() - 1.5 * SPRITE_HEIGHT > peachY || peachY > getY() + 1.5 * SPRITE_HEIGHT) return;

	// 6
	int peachX = (int)world()->getPeach()->getX();
	setDirection(peachX - getX() > 0 ? 0 : 180);

	// 7
	if (m_firingDelay > 0)
	{
		m_firingDelay--;
		return;
	}

	// 8
	if (getX() - 8 * SPRITE_WIDTH < peachX && peachX < getX() + 8 * SPRITE_WIDTH)
	{
		Actor* fireball = new PiranhaFireBall(world(), getX(), getY(), getDirection());
		world()->createActor(fireball);
		delete fireball;
		world()->playSound(SOUND_PIRANHA_FIRE);
		m_firingDelay = 40;
	}
}

void Piranha::bonk(Actor* bonker, bool invincible)
{
	if (bonker->isPeach() && invincible == true)
	{
		world()->playSound(SOUND_PLAYER_KICK);
		world()->increaseScore(100);
		die();
	}
}

void Piranha::damage()
{
	world()->increaseScore(100);
	die();
}

// GOOPA IMPLEMENTATION
Goomba::Goomba(StudentWorld* world, int x, int y, int dir)
	: Creature(world, IID_GOOMBA, x, y, dir, 0) {}

void Goomba::doSomething()
{
	// 1
	if (isAlive() == false) return;

	// 2
	Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isPeach())
	{
		overlap->bonk(this, false);
		return;
	}

	// 3 & 4?
	int newX = (int)getX();
	if (getDirection() == 0)
	{
		Actor* overlap = world()->overlap(getX() + 1, getX() + SPRITE_WIDTH, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
		Actor* overlap2 = world()->overlap(getX() + SPRITE_WIDTH, getX() + SPRITE_WIDTH * 2 - 1, getY() - SPRITE_HEIGHT, getY() - 1, this, true); // ?
		if (overlap->isBlocker() || !overlap2->isBlocker())
		{
			newX--;
			setDirection(180);
		}
		else newX++;
	}
	else
	{
		Actor* overlap = world()->overlap(getX() - 1, getX() + SPRITE_WIDTH - 2, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
		Actor* overlap2 = world()->overlap(getX() - SPRITE_WIDTH, getX() - 1, getY() - SPRITE_HEIGHT, getY() - 1, this, true); // ?
		//Actor* overlap2 = world()->overlap(getX() - 1, getX() - 1, getY() - SPRITE_HEIGHT, getY() - 1, this); // ?
		if (overlap->isBlocker() || !overlap2->isBlocker())
		{
			setDirection(0);
			newX++;
		}
		else newX--;
	}

	// 5
	overlap = world()->overlap(newX, newX + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isBlocker() == false) moveTo(newX, getY());
}

void Goomba::bonk(Actor* bonker, bool invincible)
{
	if (bonker->isPeach() && invincible == true)
	{
		world()->playSound(SOUND_PLAYER_KICK);
		world()->increaseScore(100);
		die();
	}
}

void Goomba::damage()
{
	world()->increaseScore(100);
	die();
}



// KOOPA IMPLEMENTATION

Koopa::Koopa(StudentWorld* world, int x, int y, int dir)
	: Creature(world, IID_KOOPA, x, y, dir, 0) {}

void Koopa::doSomething()
{
	// 1
	if (isAlive() == false) return;

	// 2
	Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isPeach())
	{
		overlap->bonk(this, false);
		return;
	}

	// 3 & 4?
	int newX = (int)getX();
	if (getDirection() == 0)
	{
		Actor* overlap = world()->overlap(getX() + 1, getX() + SPRITE_WIDTH, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
		Actor* overlap2 = world()->overlap(getX() + SPRITE_WIDTH, getX() + SPRITE_WIDTH * 2 - 1, getY() - SPRITE_HEIGHT, getY() - 1, this, true); // ?
		if (overlap->isBlocker() || !overlap2->isBlocker())
		{
			newX--;
			setDirection(180);
		}
		else newX++;
	}
	else
	{
		Actor* overlap = world()->overlap(getX() - 1, getX() + SPRITE_WIDTH - 2, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
		Actor* overlap2 = world()->overlap(getX() - SPRITE_WIDTH, getX() - 1, getY() - SPRITE_HEIGHT, getY() - 1, this, true); // ?
		//Actor* overlap2 = world()->overlap(getX() - 1, getX() - 1, getY() - SPRITE_HEIGHT, getY() - 1, this); // ?
		if (overlap->isBlocker() || !overlap2->isBlocker())
		{
			setDirection(0);
			newX++;
		}
		else newX--;
	}

	// 5
	overlap = world()->overlap(newX, newX + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isBlocker() == false) moveTo(newX, getY());
}

void Koopa::bonk(Actor* bonker, bool invincible)
{
	if (bonker->isPeach() && invincible == true)
	{
		world()->playSound(SOUND_PLAYER_KICK);
		world()->increaseScore(100);
		die();
		Actor* shell = new Shell(world(), getX(), getY(), getDirection());
		world()->createActor(shell);
		delete shell;
		
	}
}

void Koopa::damage()
{
	world()->increaseScore(100);
	die();
	Actor* shell = new Shell(world(), getX(), getY(), getDirection());
	world()->createActor(shell);
	delete shell;
}



// SHELL IMPLEMENTATION

Shell::Shell(StudentWorld* world, int x, int y, int dir)
	: Actor(world, IID_SHELL, x, y, dir, 1) {}

void Shell::doSomething()
{
	Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isDamageable() && !overlap->isPeach())
	{
		overlap->damage();
		die();
		return;
	}
	overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY() - 1, getY(), this, true);
	if (overlap->isBlocker() == false) moveTo(getX(), getY() - 2);
	int newX = (int)getX() + (90 - getDirection()) / 90 * 2;
	overlap = world()->overlap(newX, newX + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isBlocker())
	{
		die();
		return;
	}
	moveTo(newX, getY());
}


// PEACH IMPLEMENTATION

Peach::Peach(StudentWorld* world, int x, int y, int dir, int depth)
	: Creature(world, IID_PEACH, x, y, 0, 0),
	m_hp(1), m_invincible(0), m_recharge(0), m_jumpLeft(0), m_jumping(0), m_starPower(0), m_shootPower(0), m_jumpPower(0) {}

void Peach::bonk(Actor* bonker, bool starPower)
{
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

void Peach::doSomething()
{
	// 1
	if (isAlive() == false) return;

	// 2 & 3
	if (m_starPower)
	{
		m_invincible--;
		if (m_invincible == 0) m_starPower = false;
	}
	else if (m_invincible > 0) m_invincible--;

	// 4
	if (m_recharge > 0) m_recharge--;

	// 5
	Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (this != overlap)
		overlap->bonk(this, m_starPower);

	// 6
	if (m_jumping && m_jumpLeft > 0)
	{
		double newY = getY() + 4;
		Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, newY, newY + SPRITE_HEIGHT - 1, this, false);
		if (overlap->isBlocker())
		{
			overlap->bonk(this, m_starPower);
			m_jumpLeft = 0;
			m_jumping = false;
		}
		else
		{
			moveTo(getX(), newY);
			m_jumpLeft--;
		}
	}

	// 7
	
	else
	{
		Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY() - 3, getY(), this, true);
		if (overlap->isBlocker() == false) moveTo(getX(), getY() - 4);
	}
	
	// 8
	int keyPressed;
	if (world()->getKey(keyPressed))
	{
		// 9
		switch (keyPressed)
		{
		case (KEY_PRESS_LEFT):
		{
			setDirection(180);
			int newX = (int)getX() - 4;
			Actor* overlap = world()->overlap(newX, newX + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
			if (overlap->isBlocker()) overlap->bonk(this, m_starPower);
			else moveTo(newX, getY());
		} break;
			
		case (KEY_PRESS_RIGHT):
		{
			setDirection(0);
			int newX = (int)getX() + 4;
			Actor* overlap = world()->overlap(newX, newX + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
			if (overlap->isBlocker()) overlap->bonk(this, m_starPower);
			else moveTo(newX, getY());
		} break;
			
		case (KEY_PRESS_UP):
		{
			Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY() - 1, getY(), this, false);
			if (overlap->isBlocker())
			{
				if (m_jumpPower == true) m_jumpLeft = 12;
				else m_jumpLeft = 8;
				world()->playSound(SOUND_PLAYER_JUMP);
				m_jumping = true;
			}
		} break;
		case (KEY_PRESS_SPACE):
		{
			if (m_shootPower && m_recharge == 0)
			{
				world()->playSound(SOUND_PLAYER_FIRE);
				m_recharge = 8;
				int newX = getDirection() == 0 ? (int)getX() + 4 : (int)getX() - 4;
				Actor* fireball = new PeachFireBall(world(), newX, getY(), getDirection());
				world()->createActor(fireball);
				delete fireball;
			}
		} break;
		}
	}
}




// BLOCK IMPLEMENTATION

Block::Block(StudentWorld* world, int x, int y, int goodie)
	: Blocker(world, IID_BLOCK, x, y, 0, 2), m_goodie(goodie){}

void Block::bonk(Actor* bonker, bool starPower)
{
	if (m_goodie != 0)
	{
		world()->playSound(SOUND_POWERUP_APPEARS);
		Actor* powerUp;
		switch (m_goodie)
		{
		case 1: powerUp = new Star(world(), getX(), getY() + 8); break;
		case 2: powerUp = new Flower(world(), getX(), getY() + 8); break;
		case 3: powerUp = new Mushroom(world(), getX(), getY() + 8); break;
		}
		world()->createActor(powerUp);
		delete powerUp;
		m_goodie = 0;
	}
	else world()->playSound(SOUND_PLAYER_BONK);
}


// PIPE IMPLEMENTATION
Pipe::Pipe(StudentWorld* world, int x, int y)
	: Blocker(world, IID_PIPE, x, y, 0, 2) {}

void Pipe::bonk(Actor* bonker, bool starPower)
{
	world()->playSound(SOUND_PLAYER_BONK);
}




// POWERUPS IMPLEMENTATION

Mushroom::Mushroom(StudentWorld* world, int x, int y) 
	: Actor(world, IID_MUSHROOM, x, y, 0, 1) {}

void Mushroom::doSomething()
{
	powerDo(3);

}

Flower::Flower(StudentWorld* world, int x, int y)
	: Actor(world, IID_FLOWER, x, y, 0, 1) {}

void Flower::doSomething()
{
	powerDo(2);
}

Star::Star(StudentWorld* world, int x, int y)
	: Actor(world, IID_STAR, x, y, 0, 1) {}

void Star::doSomething()
{
	powerDo(1);
}


// PROJECTILES IMPLEMENTATION
PeachFireBall::PeachFireBall(StudentWorld* world, int x, int y, int dir)
	: Actor(world, IID_PEACH_FIRE, x, y, dir, 1) {}

void PeachFireBall::doSomething()
{
	Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isDamageable() && !overlap->isPeach())
	{
		overlap->damage();
		die();
		return;
	}
	overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY() - 1, getY(), this, true);
	if (overlap->isBlocker() == false) moveTo(getX(), getY() - 2);
	int newX = (int)getX() + (90 - getDirection()) / 90 * 2;
	overlap = world()->overlap(newX, newX + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isBlocker())
	{
		die();
		return;
	}
	moveTo(newX, getY());

}

PiranhaFireBall::PiranhaFireBall(StudentWorld* world, int x, int y, int dir)
	: Actor(world, IID_PIRANHA_FIRE, x, y, dir, 1) {}

void PiranhaFireBall::doSomething()
{
	Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isPeach())
	{
		overlap->bonk(this, false);
		die();
		return;
	}
	overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY() - 1, getY(), this, true);
	if (overlap->isBlocker() == false) moveTo(getX(), getY() - 2);
	int newX = (int)getX() + (90 - getDirection()) / 90 * 2;
	overlap = world()->overlap(newX, newX + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isBlocker())
	{
		die();
		return;
	}
	moveTo(newX, getY());

}


// LEVEL ITEMS IMPLEMENTATION

Flag::Flag(StudentWorld* world, int x, int y, int dir)
	: Actor(world, IID_FLAG, x, y, 0, 1) {}

void Flag::doSomething()
{
	if (isAlive() == false) return;
	Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isPeach())
	{
		world()->increaseScore(1000);
		world()->levelDone();
		die();
		return;
	}
}

Mario::Mario(StudentWorld* world, int x, int y, int dir)
	: Actor(world, IID_MARIO, x, y, 0, 1) {}

void Mario::doSomething()
{
	if (isAlive() == false) return;
	Actor* overlap = world()->overlap(getX(), getX() + SPRITE_WIDTH - 1, getY(), getY() + SPRITE_HEIGHT - 1, this, false);
	if (overlap->isPeach())
	{
		world()->increaseScore(1000);
		world()->gameWon();
		die();
		return;
	}
}




