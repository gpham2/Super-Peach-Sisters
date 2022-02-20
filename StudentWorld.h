#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>

#include <list>
using namespace std;



// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class Peach;
class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  ~StudentWorld();

  // virtuals
  virtual int init();
  virtual int move();
  virtual void cleanUp();

  // Accessors
  Peach* getPeach() const { return m_peach; }
  Actor* overlap(double sx, double ex, double sy, double ey, Actor* ignore, bool ignoreNonBlockers) const;
  
  // Mutators
  void gameWon() { m_gameWon = true; }
  void levelDone() { m_levelDone = true; }
  void createActor(Actor* actorType);
  void display();

private:
	list<Actor*> m_actors;
	Peach* m_peach;
	bool m_levelDone;
	bool m_gameWon;
};

#endif // STUDENTWORLD_H_
