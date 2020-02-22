#include "GraphObject.h"

class GameWorld;
class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* sWorld, const int imageID, double startX, double startY,
		Direction dir, int depth);
	virtual void doSomething() = 0;

	virtual bool canMove() const; 
	virtual bool canFinish() const;
	bool isAlive() const;
	virtual bool canDieIndependently() const;
	virtual void react();
	virtual void infect();
	virtual bool isAnimate() const; 
	virtual bool projectilesCanPass() const; 
	virtual bool movingActorsCanPass() const;
	virtual bool canDieFromFlame() const;
	void kill();
	StudentWorld* getWorld() const;

private:
	bool m_isAlive;
	StudentWorld * sw;
};

class StableObject : public Actor
{
public:
	StableObject(StudentWorld* sWorld, int imageID, double startX, double startY, int depth);
	virtual bool isAnimate() const;
	virtual bool projectilesCanPass() const;
	virtual bool canDieFromFlame() const;
};

class Wall : public StableObject
{
public:
	Wall(StudentWorld* sWorld, double startX, double startY);
	virtual void doSomething();
	virtual bool movingActorsCanPass() const;
private:

};

class Exit : public StableObject
{
public:
	Exit(StudentWorld* sWorld, double x, double y);
	virtual void doSomething();
private:

};

class Pit : public StableObject
{
public:
	Pit(StudentWorld* sWorld, double x, double y);
	virtual void doSomething();
	virtual bool projectilesCanPass() const;
private:
};

class Landmine : public Actor
{
public:
	Landmine(StudentWorld* sWorld, double x, double y);
	virtual void doSomething();
	virtual void react();
private:
	int m_safetyTicks;
};

class Goodie : public Actor
{
public: 
	Goodie(StudentWorld* sWorld, const int imageID, double startX, double startY);
	bool act();
private:
};

class VaccineGoodie : public Goodie
{
public:
	VaccineGoodie(StudentWorld* sWorld, double startX, double startY);
	virtual void doSomething();
private:

};

class LandmineGoodie : public Goodie
{
public:
	LandmineGoodie(StudentWorld* sWorld, double startX, double startY);
	virtual void doSomething();
private:

};

class GasCanGoodie : public Goodie
{
public:
	GasCanGoodie(StudentWorld* sWorld, double startX, double startY);
	virtual void doSomething();
private:

};

class Projectile : public Actor
{
public:
	Projectile(StudentWorld* sWorld, const int imageID, double startX, double startY, Direction d);
	void decreaseCount();
	virtual bool isAnimate() const;
	virtual bool canDieIndependently() const;
	virtual bool canDieFromFlame() const;
	int getCount() const;
private:
	int m_count;
};

class Flame : public Projectile
{
public:
	Flame(StudentWorld* sWorld, double startX, double startY, Direction d);
	virtual void doSomething();
private:
};

class Vomit : public Projectile
{
public: 
	Vomit(StudentWorld* sWorld, double startX, double startY, Direction d);
	virtual void doSomething();
private:
};

class Human : public Actor
{
public:
	Human(StudentWorld* sWorld, const int IID, double startX, double startY);
	virtual bool turnBasedParalysis();
	virtual bool canMove() const;
	virtual bool movingActorsCanPass() const;
	bool okToMove(double newX, double newY) const;
	bool okToMoveDir(int numSteps, Direction d);
	void follow(int numSteps, Direction d);
	Direction followDirection(Actor* closest);
private:
	bool m_paralyzed;
};

class LiveHuman : public Human
{
public:
	LiveHuman(StudentWorld* sWorld, const int IID, double startX, double startY);
	virtual bool canFinish() const;
	bool isInfected() const;
	void infect();
	int infectionNum() const;
	void resetInfectionStatus();
	virtual bool ifInfectedProcess();
private:	
	bool m_infectionStatus;
	int m_infectionCount;
};

class Player : public LiveHuman
{
public:
	Player(StudentWorld* sWorld, double startX, double startY);
	virtual void doSomething();
	bool okToMove(double newX, double newY) const;
private:
};

class Citizen : public LiveHuman
{
public:
	Citizen(StudentWorld* sWorld, double startX, double startY);
	virtual void doSomething();
	virtual void react();
private:
};

class Zombie : public Human
{
public:
	Zombie(StudentWorld* sWorld, const int imageID, double startX, double startY);
	bool defaultVomit();
	void moveAlongRandomPlan();
	void moveAlongFollowPlan(Actor* closest);
private:
	int m_movementPlan;
};

class DumbZombie : public Zombie
{
public:
	DumbZombie(StudentWorld* sWorld, double startX, double startY);
	virtual void react();
	virtual void doSomething();
private:
	
};

class SmartZombie : public Zombie
{
public:
	SmartZombie(StudentWorld* sWorld, double startX, double startY);
	virtual void react();
	virtual void doSomething();
private:

};