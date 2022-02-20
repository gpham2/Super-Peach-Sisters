#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <list>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    m_levelDone = false;
    Level lev(assetPath());
    ostringstream levelNum;
    levelNum.fill('0');
    levelNum << setw(2) << getLevel();
    string levelFile = "level" + levelNum.str() + ".txt";

    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result != Level::load_success) return GWSTATUS_LEVEL_ERROR;

    for (int lx = 0; lx < 32; lx++)
    {
        for (int ly = 0; ly < 32; ly++)
        {
            Level::GridEntry ge = lev.getContentsOf(lx, ly);
            int x = lx * SPRITE_WIDTH;
            int y = ly * SPRITE_HEIGHT;
            switch (ge)
            {
            case Level::pipe: m_actors.push_back(new Pipe(this, x, y)); break;
            case Level::block: m_actors.push_back(new Block(this, x, y, false)); break;
            case Level::mushroom_goodie_block: m_actors.push_back(new Block(this, x, y, 3)); break;
            case Level::flower_goodie_block: m_actors.push_back(new Block(this, x, y, 2)); break;
            case Level::star_goodie_block: m_actors.push_back(new Block(this, x, y, 1)); break;
            case Level::piranha: m_actors.push_back(new Piranha(this, x, y, randInt(0, 1) * 180)); break;
            case Level::goomba: m_actors.push_back(new Goomba(this, x, y, randInt(0,1) * 180)); break;
            case Level::koopa: m_actors.push_back(new Koopa(this, x, y, randInt(0, 1) * 180)); break;
            case Level::peach: m_peach = new Peach(this, x, y, 0, 0); m_actors.push_back(m_peach); break;
            case Level::flag: m_actors.push_back(new Flag(this, x, y, 0)); break;
            case Level::mario: m_actors.push_back(new Mario(this, x, y, 0)); break;
             //TBC...
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    //decLives();
    //return GWSTATUS_PLAYER_DIED;

    // 1
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
        (*it)->doSomething();
        
    if (m_peach->isAlive() == false)
    {
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }

    // 2
    if (m_levelDone == true)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    // 3
    if (m_gameWon == true)
    {
        playSound(SOUND_GAME_OVER);
        return GWSTATUS_PLAYER_WON;
    }

    // 4
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end();)
    {
        if ((*it)->isAlive() == false)
        {
            delete(*it);
            list<Actor*>::iterator it2 = it;
            it++;
            m_actors.erase(it2);
        }
        else it++;
    }



    // 5 TODO
    display();

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end();)
    {
        delete(*it);
        list<Actor*>::iterator it2 = it;
        it++;
        m_actors.erase(it2);
    }
}


Actor* StudentWorld::overlap(double x, double xx, double y, double yy, Actor* ignore, bool ignoreNonBlockers) const
{
    int sx = (int) x; int ex = (int) xx; int sy = (int) y; int ey = (int) yy;
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if (ignore == (*it)) continue;
        if (ignoreNonBlockers == true && !(*it)->isBlocker()) continue;
        int x2 = (int)(*it)->getX() + SPRITE_WIDTH - 1;
        int y2 = (int)(*it)->getY() + SPRITE_HEIGHT - 1;
        if ((sx <= (int)(*it)->getX() && (int)(*it)->getX() <= ex && sy <= (int)(*it)->getY() && (int)(*it)->getY() <= ey) ||
            (sx <= x2 && x2 <= ex && sy <= (int)(*it)->getY() && (int)(*it)->getY() <= ey) ||
            (sx <= (int)(*it)->getX() && (int)(*it)->getX() <= ex && sy <= y2 && y2 <= ey) ||
            (sx <= x2 && x2 <= ex && sy <= y2 && y2 <= ey))
            return (*it);
    }
    return ignore;
}

void StudentWorld::createActor(Actor* actorType)
{
    m_actors.push_front(actorType->create());
}

void StudentWorld::display()
{
    ostringstream display;

    string star = m_peach->hasStar() ? " StarPower!" : "";
    string shoot = m_peach->hasShoot() ? " ShootPower!" : "";
    string jump = m_peach->hasJump() ? " JumpPower!" : "";
    
    display << "Lives: " << getLives();
    display << "  Level: ";
    display.fill('0');
    display << setw(2) << getLevel();
    display << "  Points: ";
    display.fill('0');
    display << setw(6) << getScore();
    display << star << shoot << jump;


    setGameStatText(display.str());
}


