#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <list>
using namespace std;

class Actor;
class Peach;
class StudentWorld : public GameWorld
{
public:
	
	// mandatories
	StudentWorld(std::string assetPath);
	~StudentWorld() { cleanUp(); }
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	// accessors
	bool peachInPiranhaRange(Actor* piranha);
	void piranhaFacePeach(Actor* piranha, int& peachX);
	bool peachHasStar();
	void peachGetStar();
	void peachGetShoot();
	void peachGetJump();
	void display();

	// mutators
	void win() { m_gameWon = true; }
	void nextLevel() { m_levelDone = true; }
	bool isPeachHere(Actor* actor, bool damage);
	bool bonkPeach(Actor* bonker);
	bool bonkAllNonBlocks(Actor* bonker, bool onlyOne);
	bool isBlockingObjectAt(int sx, int ex, int sy, int ey, bool bonk);
	void createActor(Actor* actorType);

private:
	list<Actor*> m_actors;
	Peach* m_peach;
	bool m_levelDone;
	bool m_gameWon;
};

#endif // STUDENTWORLD_H_
