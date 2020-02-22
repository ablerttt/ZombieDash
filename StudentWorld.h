#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <list>
#include <string>

class Actor;
class Player;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
	bool loadLevel(int level);
	void updateItems(char c);
	int numVacc() const;
	int numFlames() const;
	int numMines() const;
	int numCitizens() const;
	int numZombies() const;
	Player* pointertoMC() const;

	void decrementCitizens();
	void decrementZombies();
	void incrementZombies();
	double distanceToPenelope(double x, double y) const;

	bool occupied(double x, double y, bool human);
	bool humanOccupied(double newX, double newY, double oldX, double oldY);
	bool mcoverlap(double x, double y);
	bool overlaps(double x1, double y1, double x2, double y2) const;

	void killAllKillableActors(double x, double y, bool flame);
	bool completeAllCompletableActors(double x, double y);
	void infectAllInfectableActors(double x, double y);
	bool killMovableActors(double x, double y);

	void updateMCInfectionStatus(int infected);
	double closestRealHuman(double x, double y);
	double closestZombie(double x, double y);
	Actor* closestRealHumanPointer(double x, double y);
	double distanceBetween(double x1, double y1, double x2, double y2) const;

	void newProjectile(int type, double x, double y, int d);
	void newObject(double x, double y, char type);
	void newZombie(double x, double y, bool smart);

	void levelUpdate(int i);
	virtual ~StudentWorld();

private:
	std::list<Actor*> m_actors;
	Player* penelope;
	int m_vacc;
	int m_flames;
	int m_mines;
	int m_infected;

	int m_nCitizens;
	int m_nZombies;
	int m_levelStatus;
};

#endif // STUDENTWORLD_H_

