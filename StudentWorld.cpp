#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <list>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

bool StudentWorld::loadLevel(int level)
{
	Level lev(assetPath());
	string levelFile;
	stringstream ss;
	ss << level;
	string levelString = ss.str();
	char period = '.';
	if (level < 10)
		levelFile = "level0" + levelString + ".txt";
	else
		levelFile = "level" + levelString + ".txt";
	
	Level::LoadResult result = lev.loadLevel(levelFile);
	if (result == Level::load_fail_file_not_found)
	{
		cerr << "Cannot find " << levelFile << " data file" << endl;
		return false;
	}
	else if (result == Level::load_fail_bad_format)
	{
		cerr << "Your level was improperly formatted" << endl;
		return false;
	}
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level" << endl;
		for (int i = 0; i < LEVEL_WIDTH; i++)
		{
			for (int j = 0; j < LEVEL_HEIGHT; j++)
			{ 
				Level::MazeEntry ge = lev.getContentsOf(i, j); // level_x=5, level_y=10
				switch (ge) // so x=80 and y=160
				{
				case Level::empty:
					break;
				case Level::wall:
					m_actors.push_back(new Wall(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					break;	
				case Level::player:
					penelope = new Player(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT);
					break;


				case Level::smart_zombie:
					m_actors.push_back(new SmartZombie(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT));
					m_nZombies++;
					break;
				case Level::dumb_zombie:
					m_actors.push_back(new DumbZombie(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT));
					m_nZombies++;
				 	break;
				case Level::exit:
					m_actors.push_back(new Exit(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT)); 
					break;
				case Level::pit:
					m_actors.push_back(new Pit(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT));
					break;
				case Level::citizen:
					m_actors.push_back(new Citizen(this, i*SPRITE_WIDTH, j*SPRITE_HEIGHT));
					m_nCitizens++;
					break;
				case Level::vaccine_goodie:
					m_actors.push_back(new VaccineGoodie(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					break;
				case Level::gas_can_goodie:
					m_actors.push_back(new GasCanGoodie(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					break;
				case Level::landmine_goodie:
					m_actors.push_back(new LandmineGoodie(this, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
					break;
				}
			}
		}
	}
	return true;
}

void StudentWorld::updateItems(char c)
{
	switch (c)
	{
	case 'v':
		m_vacc++;
		break;
	case 'V':
		m_vacc--;
		break;
	case 'm':
		m_mines += 2;
		break;
	case 'M':
		m_mines--;
		break;
	case 'f':
		m_flames+=5;
		break;
	case 'F':
		m_flames--;
		break;
	default:
		break;
	}
}

int StudentWorld::numVacc() const
{
	return m_vacc;
}

int StudentWorld::numFlames() const
{
	return m_flames;
}

int StudentWorld::numMines() const
{
	return m_mines;
}

int StudentWorld::numCitizens() const
{
	return m_nCitizens;
}

int StudentWorld::numZombies() const
{
	return m_nZombies;
}

Player * StudentWorld::pointertoMC() const
{
	return penelope;
}

void StudentWorld::decrementCitizens()
{
	m_nCitizens--;
}

void StudentWorld::decrementZombies()
{
	m_nZombies--;
}

void StudentWorld::incrementZombies()
{
	m_nZombies++;
}

double StudentWorld::distanceToPenelope(double x, double y) const
{
	return sqrt((penelope->getX() - x) * (penelope->getX() - x) + (penelope->getY() - y) * (penelope->getY() - y));
}

//used to check if the current spot is occupied. 
bool StudentWorld::occupied(double x, double y, bool human)
{
	//human is true -> used for player, zombie, citizen
	//human is false -> used for flames, vomit
	list<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		if ((human && (*it)->movingActorsCanPass()) || (!human && (*it)->projectilesCanPass()))
		{
			it++;
			continue;
		}

		double actorX = (*it)->getX();
		double actorY = (*it)->getY();

			
		if (x + SPRITE_WIDTH - 1 >= actorX && x <= actorX + SPRITE_WIDTH - 1 &&
			y + SPRITE_HEIGHT - 1 >= actorY && y <= actorY + SPRITE_HEIGHT - 1)
		{
			return true;
		}

		it++;
	}

	return false;
}

//used to check if a moving actor can step onto the next space. takes into account the current position of the actor
//to ensure that the bool won't return false (since the new position will overlap with the current one)
bool StudentWorld::humanOccupied(double newX, double newY, double oldX, double oldY)
{
	list<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		if ((*it)->movingActorsCanPass())
		{
			it++;
			continue;
		}

		double actorX = (*it)->getX();
		double actorY = (*it)->getY();

		if ((*it)->getX() == oldX && (*it)->getY() == oldY)
		{
			it++;
			continue;
		}


		if (newX + SPRITE_WIDTH - 1 >= actorX && newX <= actorX + SPRITE_WIDTH - 1 &&
			newY + SPRITE_HEIGHT - 1 >= actorY && newY <= actorY + SPRITE_HEIGHT - 1)
		{
			return true;
		}

		it++;
	}
	if (newX + SPRITE_WIDTH - 1 >= penelope->getX() && newX <= penelope->getX() + SPRITE_WIDTH - 1 &&
		newY + SPRITE_HEIGHT - 1 >= penelope->getY() && newY <= penelope->getY() + SPRITE_HEIGHT - 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool StudentWorld::mcoverlap(double x, double y) 
{
	return overlaps(penelope->getX(), penelope->getY(), x, y);
}

bool StudentWorld::overlaps(double x1, double y1, double x2, double y2) const
{
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) <= 10.0;
}


void StudentWorld::killAllKillableActors(double x, double y, bool flame)
{
	list<Actor*>::iterator it = m_actors.begin();

	while (it != m_actors.end())
	{

		if (overlaps(x, y, (*it)->getX(), (*it)->getY()) && (*it)->isAnimate() 
			&& (*it)->isAlive())
		{
			if (flame && (*it)->canDieFromFlame())
			{
				(*it)->react();
				(*it)->kill();
				it++;
			}
			else if (!flame && (*it)->canMove())
			{
				(*it)->react();
				(*it)->kill();
				it++;
			}
			else
				it++;
		}
		else
			it++;
	}

	if (overlaps(x, y, penelope->getX(), penelope->getY()))
		m_levelStatus = 1; //game ended
}

bool StudentWorld::completeAllCompletableActors(double x, double y)
{
	list<Actor*>::iterator it = m_actors.begin();
	bool result = false;
	while (it != m_actors.end())
	{

		if (overlaps(x, y, (*it)->getX(), (*it)->getY()) && (*it)->canFinish())
		{
			result = true;
			(*it)->kill();
			m_nCitizens--;
			it++;
		}
		else
			it++;
	}
	return result;
}

void StudentWorld::infectAllInfectableActors(double x, double y)
{
	list<Actor*>::iterator it = m_actors.begin();

	while (it != m_actors.end())
	{
		if (!(*it)->canFinish())
		{
			it++;
			continue;
		}

		if (overlaps(x, y, (*it)->getX(), (*it)->getY()))
		{
			(*it)->infect();
		}
		it++;
	}

	if (mcoverlap(x, y))
		penelope->infect();
}

bool StudentWorld::killMovableActors(double x, double y)
{
	bool result = false;
	list<Actor*>::iterator it = m_actors.begin();

	while (it != m_actors.end())
	{
		if (!(*it)->canMove())
		{
			it++;
			continue;
		}

		if (overlaps(x, y, (*it)->getX(), (*it)->getY()))
		{
			result = true;
		}
		it++;
	}
	return result;
}

void StudentWorld::updateMCInfectionStatus(int infected)
{
	m_infected = infected;
}

double StudentWorld::closestRealHuman(double x, double y)
{
	list<Actor*>::iterator it = m_actors.begin();
	double minDistance = distanceToPenelope(x, y);

	while (it != m_actors.end())
	{
		if (!(*it)->canFinish())
		{
			it++;
			continue;
		}

		double currentDistance = distanceBetween(x, y, (*it)->getX(), (*it)->getY());
		if (currentDistance < minDistance)
		{
			minDistance = currentDistance;
		}
		it++;
	}
	return minDistance;
}

double StudentWorld::closestZombie(double x, double y)
{
	list<Actor*>::iterator it = m_actors.begin();
	double minDistance = 999999999999;

	while (it != m_actors.end())
	{
		if (!(*it)->canMove() || (*it)->canFinish())
		{
			it++;
			continue;
		}

		double currentDistance = distanceBetween(x, y, (*it)->getX(), (*it)->getY());
		if (currentDistance < minDistance)
		{
			minDistance = currentDistance;
		}
		it++;
	}
	return minDistance;
}

Actor * StudentWorld::closestRealHumanPointer(double x, double y)
{
	list<Actor*>::iterator it = m_actors.begin();
	double minDistance = distanceToPenelope(x, y);
	Actor* result = penelope;

	while (it != m_actors.end())
	{
		if (!(*it)->canFinish())
		{
			it++;
			continue;
		}

		double currentDistance = distanceBetween(x, y, (*it)->getX(), (*it)->getY());
		if (currentDistance < minDistance)
		{
			minDistance = currentDistance;
			result = (*it);
		}
		it++;
	}
	return result;
}

double StudentWorld::distanceBetween(double x1, double y1, double x2, double y2) const
{
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

void StudentWorld::newProjectile(int type, double x, double y, int d)
{
	//1 stands for FLAMES, 0 stands for VOMIT
	if (type == 1)
	{
		m_actors.push_back(new Flame(this, x, y, d));
	}
	else if (type == 0)
	{
		playSound(SOUND_ZOMBIE_VOMIT);
		m_actors.push_back(new Vomit(this, x, y, d));
	}
	else
		cerr << "wrong type of projectile passed through" << endl;
}

void StudentWorld::newObject(double x, double y, char type)
{
	if (type == 'l')
	{
		m_actors.push_back(new Landmine(this, x, y));
	}
	else if (type == 'p')
	{
		m_actors.push_back(new Pit(this, x, y));
	}
	else if (type == 'v')
	{
		m_actors.push_back(new VaccineGoodie(this, x, y));
	}
}

void StudentWorld::newZombie(double x, double y, bool smart)
{
	m_nZombies++;
	if (smart)
		m_actors.push_back(new SmartZombie(this, x, y));
	else
		m_actors.push_back(new DumbZombie(this, x, y));
}

void StudentWorld::levelUpdate(int i)
{
	if (i > -1)
		m_levelStatus = i;
}


StudentWorld::~StudentWorld()
{
	cleanUp();
}


int StudentWorld::init()
{
	m_vacc = 0;
	m_flames = 0;
	m_mines = 0;
	m_infected = 0;
	m_nCitizens = 0;
	m_nZombies = 0;
	m_levelStatus = -1;

	if (loadLevel(getLevel()))
		return GWSTATUS_CONTINUE_GAME;
	else
		return GWSTATUS_PLAYER_WON;
}
	
int StudentWorld::move()
{
	penelope->doSomething();
	list<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		(*it)->doSomething();
		it++;

		if (!penelope->isAlive())
		{
			return GWSTATUS_PLAYER_DIED;
		}

	}
	it = m_actors.begin();
	while (it != m_actors.end())	
	{
		if (((*it)->isAnimate() && !(*it)->isAlive())|| ((*it)->canDieIndependently() && !(*it)->isAlive()))
		{
			delete (*it);
			it = m_actors.erase(it);
		}
		else
			it++;
	}

	ostringstream oss;
	oss.setf(ios::fixed);
	oss.precision(5);
	oss.fill('0');
	oss << "Score: ";
	if (getScore() < 0)
		oss << "-" << setw(5) << (getScore() * -1);
	else
		oss << setw(6) << getScore();
	oss.fill(' ');
	oss << setw(9) << "Level: " << getLevel() << setw(9) << "Lives: " << getLives() << setw(12) 
		<< "Vaccines: " << m_vacc <<setw(10) << "Flames: " << m_flames << setw(9) << "Mines: " 
		<< m_mines << setw(12) << "Infected: " << m_infected;
    
	setGameStatText(oss.str());

	if (m_levelStatus == 0)
	{
		return GWSTATUS_FINISHED_LEVEL;
	}
	else if (m_levelStatus == 1)
	{
		decLives();
		playSound(SOUND_PLAYER_DIE);
		return GWSTATUS_PLAYER_DIED;
	}

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	list<Actor*>::iterator it = m_actors.begin();
	while (it != m_actors.end())
	{
		delete (*it);
		it = m_actors.erase(it);
	}
	delete penelope;
	penelope = nullptr;
}
