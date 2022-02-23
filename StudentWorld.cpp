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

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_peach = nullptr;
}

int StudentWorld::init()
{
    // initialize variables
    m_levelDone = false;
    m_gameWon = false;

    // get level in text format
    Level lev(assetPath());
    ostringstream levelNum;
    levelNum.fill('0');
    levelNum << setw(2) << getLevel();
    string levelFile = "level" + levelNum.str() + ".txt";
    Level::LoadResult result = lev.loadLevel(levelFile);
    if (result != Level::load_success) return GWSTATUS_LEVEL_ERROR;

    // add in actors
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
            case Level::peach: m_peach = new Peach(this, x, y, 0, 0); m_actors.push_back(m_peach); break;
            case Level::goomba: m_actors.push_back(new Goomba(this, x, y, randInt(0, 1) * 180)); break;
            case Level::koopa: m_actors.push_back(new Koopa(this, x, y, randInt(0, 1) * 180)); break;
            case Level::piranha: m_actors.push_back(new Piranha(this, x, y, randInt(0, 1) * 180)); break;
            case Level::flag: m_actors.push_back(new Flag(this, x, y)); break;
            case Level::mario: m_actors.push_back(new Mario(this, x, y)); break;
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // all actors do something
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
        (*it)->doSomething();

    // check if peach died
    if (m_peach->isAlive() == false)
    {
        playSound(SOUND_PLAYER_DIE);
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }

    // check if level is completed
    if (m_levelDone == true)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }

    // check if game is won
    if (m_gameWon == true)
    {
        playSound(SOUND_GAME_OVER);
        return GWSTATUS_PLAYER_WON;
    }

    // delete dead actors
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

    // display lives, levels, points and powerups
    display();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // delete dynamically allocated data
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end();)
    {
        delete(*it);
        list<Actor*>::iterator it2 = it;
        it++;
        m_actors.erase(it2);
    }
}


bool StudentWorld::peachInPiranhaRange(Actor* piranha)
{
    int piranhaY = (int)piranha->getY();
    int peachY = (int)m_peach->getY();
    if (piranhaY - 1.5 * SPRITE_HEIGHT > peachY || peachY > piranhaY + 1.5 * SPRITE_HEIGHT) return false;
    return true;
}

void StudentWorld::piranhaFacePeach(Actor* piranha, int& peachX)
{
    int pX = (int)m_peach->getX();
    piranha->setDirection(pX - (int)piranha->getX() > 0 ? 0 : 180);
    peachX = pX;
}

bool StudentWorld::peachCanKill(Actor* actor) { return m_peach->hasStar() && actor == m_peach; }
void StudentWorld::peachGetStar() { m_peach->getStar(); increaseScore(100); }
void StudentWorld::peachGetShoot() { m_peach->getShoot(); increaseScore(50); m_peach->setHp(); }
void StudentWorld::peachGetJump() { m_peach->getJump(); increaseScore(75); m_peach->setHp(); }

void StudentWorld::display()
{
    ostringstream display;
    
    // determine current powerups
    string star = m_peach->hasStar() ? " StarPower!" : "";
    string shoot = m_peach->hasShoot() ? " ShootPower!" : "";
    string jump = m_peach->hasJump() ? " JumpPower!" : "";

    // figure out lives, levels, and points
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

bool StudentWorld::isPeachHere(Actor* bonker, bool damage)
{
    // check if peach overlaps the actor
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it) != m_peach) continue;
        bool insideX = bonker->getX() <= (*it)->getX() + SPRITE_WIDTH - 1 && bonker->getX() + SPRITE_WIDTH - 1 >= (*it)->getX();
        bool insideY = bonker->getY() <= (*it)->getY() + SPRITE_HEIGHT - 1 && bonker->getY() + SPRITE_HEIGHT - 1 >= (*it)->getY();
        if (insideX && insideY)
        {
            if (damage) (*it)->bonk(bonker);
            return true;
        }
    }
    return false;
}

bool StudentWorld::bonkPeach(Actor* bonker)
{
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it) != m_peach) continue;
        bool insideX = bonker->getX() <= (*it)->getX() + SPRITE_WIDTH - 1 && bonker->getX() + SPRITE_WIDTH - 1 >= (*it)->getX();
        bool insideY = bonker->getY() <= (*it)->getY() + SPRITE_HEIGHT - 1 && bonker->getY() + SPRITE_HEIGHT - 1 >= (*it)->getY();
        if (insideX && insideY)
        {
            (*it)->bonk(bonker);
            return true;
        }
    }
    return false;

}

bool StudentWorld::bonkAllNonBlocks(Actor* bonker, bool onlyOne)
{
    // check coordinate of bonker and bonks every nonblock object that bonker overlaps or only 1 if specify
    int flag = false;
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it) == m_peach || (*it)->isBlocker()) continue;
        bool insideX = bonker->getX() <= (*it)->getX() + SPRITE_WIDTH - 1 && bonker->getX() + SPRITE_WIDTH - 1 >= (*it)->getX();
        bool insideY = bonker->getY() <= (*it)->getY() + SPRITE_HEIGHT - 1 && bonker->getY() + SPRITE_HEIGHT - 1 >= (*it)->getY();
        if (insideX && insideY)
        {
            int flag = true;
            if (onlyOne && (*it)->isDamageable())
            {
                (*it)->damaged();
                return true;
            }
            (*it)->bonk(bonker);
        }
    }
    return flag;
}


bool StudentWorld::isBlockingObjectAt(int sx, int ex, int sy, int ey, bool bonk)
{
    for (list<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->isBlocker() == false) continue;
        int sx2 = (int)(*it)->getX();
        int sy2 = (int)(*it)->getY();
        int ex2 = (int)(*it)->getX() + SPRITE_WIDTH - 1;
        int ey2 = (int)(*it)->getY() + SPRITE_HEIGHT - 1;
        if (sx <= ex2 && ex >= sx2 && sy <= ey2 && ey >= sy2)
        {
            if (bonk) (*it)->bonk(m_peach);
            return true;
        }
    }
    return false;
}

void StudentWorld::createActor(Actor* actorType)
{
    m_actors.push_front(actorType->create());
    delete actorType;
}