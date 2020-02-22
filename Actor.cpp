#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream>
using namespace std;

Actor::Actor(StudentWorld* sWorld, const int imageID, double startX, double startY, Direction dir, int depth)
	: GraphObject(imageID, startX, startY, dir, depth)
{
	m_isAlive = true;
	sw = sWorld;
}

bool Actor::canMove() const
{
	return false;
}

bool Actor::canFinish() const
{
	return false;
}

bool Actor::isAlive() const
{
	return m_isAlive;
}

bool Actor::canDieIndependently() const
{
	return false;
}

void Actor::kill()
{
	m_isAlive = false;
}

void Actor::react()
{
	return;
}

void Actor::infect()
{
	return;
}

bool Actor::isAnimate() const
{
	return true;
}

bool Actor::projectilesCanPass() const
{
	return true;
}

bool Actor::movingActorsCanPass() const
{
	return true;
}

bool Actor::canDieFromFlame() const
{
	return true;
}

StudentWorld * Actor::getWorld() const
{
	return sw;
}

StableObject::StableObject(StudentWorld* sWorld, int imageID, double startX, double startY, int depth)
	:Actor(sWorld, imageID, startX, startY, right, depth)
{

}

bool StableObject::isAnimate() const
{
	return false;
}

bool StableObject::projectilesCanPass() const
{
	return false;
}

bool StableObject::canDieFromFlame() const
{
	return false;
}

Wall::Wall(StudentWorld* sWorld, double startX, double startY)
	: StableObject(sWorld, IID_WALL, startX, startY, 0)
{

}

void Wall::doSomething()
{
	return;
}

bool Wall::movingActorsCanPass() const
{
	return false;
}


Exit::Exit(StudentWorld * sWorld, double x, double y)
	: StableObject(sWorld, IID_EXIT, x, y, 1)
{

}

void Exit::doSomething()
{
	if (getWorld()->numCitizens() == 0)
	{
		if (getWorld()->mcoverlap(getX(), getY()))
		{
			getWorld()->levelUpdate(0);
			getWorld()->playSound(SOUND_LEVEL_FINISHED);
		}
	}
	else
	{
		if (getWorld()->completeAllCompletableActors(getX(), getY()))
		{
			getWorld()->playSound(SOUND_CITIZEN_SAVED);
			getWorld()->increaseScore(500);
		}
	}
}

Pit::Pit(StudentWorld * sWorld, double x, double y)
	: StableObject(sWorld, IID_PIT, x, y, 0)
{

}

void Pit::doSomething()
{
	getWorld()->killAllKillableActors(getX(), getY(), false);
}

bool Pit::projectilesCanPass() const
{
	return true;
}

Landmine::Landmine(StudentWorld * sWorld, double x, double y)
	: Actor(sWorld, IID_LANDMINE, x, y, right, 1)
{	
	m_safetyTicks = 30;
}

void Landmine::doSomething()
{
	if (!isAlive())
		return;

	if (m_safetyTicks > 0)
	{
		m_safetyTicks--;
		return;
	}

	if (getWorld()->mcoverlap(getX(), getY()) || getWorld()->killMovableActors(getX(), getY()))
	{
		react();
		kill();
	}	
}

void Landmine::react()
{
	getWorld()->newProjectile(1, getX(), getY(), up);
	getWorld()->newProjectile(1, getX(), getY() + SPRITE_HEIGHT, up);
	getWorld()->newProjectile(1, getX() + SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up);
	getWorld()->newProjectile(1, getX() + SPRITE_WIDTH, getY(), up);
	getWorld()->newProjectile(1, getX() + SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up);
	getWorld()->newProjectile(1, getX(), getY() - SPRITE_HEIGHT, up);
	getWorld()->newProjectile(1, getX() - SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up);
	getWorld()->newProjectile(1, getX() - SPRITE_WIDTH, getY(), up);
	getWorld()->newProjectile(1, getX() - SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up);
	getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
	getWorld()->newObject(getX(), getY(), 'p');
}

Goodie::Goodie(StudentWorld* sWorld, const int imageID, double startX, double startY)
	: Actor(sWorld, imageID, startX, startY, right, 1)
{

}

bool Goodie::act()
{
	if (!isAlive())
		return false;

	if (getWorld()->mcoverlap(getX(), getY()))
	{
		getWorld()->increaseScore(50);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		kill();
		return true;
	}
	return false;
}

VaccineGoodie::VaccineGoodie(StudentWorld * sWorld, double startX, double startY)
	: Goodie(sWorld, IID_VACCINE_GOODIE, startX, startY)
{

}

void VaccineGoodie::doSomething()
{
	if (act())
	{
		getWorld()->updateItems('v');
	}
}

LandmineGoodie::LandmineGoodie(StudentWorld * sWorld, double startX, double startY)
	: Goodie(sWorld, IID_LANDMINE_GOODIE, startX, startY)
{
}

void LandmineGoodie::doSomething()
{
	if (act())
	{
		getWorld()->updateItems('m');
	}
}

GasCanGoodie::GasCanGoodie(StudentWorld * sWorld, double startX, double startY)
	: Goodie(sWorld, IID_GAS_CAN_GOODIE, startX, startY)
{
}

void GasCanGoodie::doSomething()
{
	if (act())
	{
		getWorld()->updateItems('f');
	}
}

Projectile::Projectile(StudentWorld * sWorld, const int imageID, double startX, double startY, Direction d)
	: Actor(sWorld, imageID, startX, startY, d, 0)
{
	m_count = 2;
}

void Projectile::decreaseCount()
{
	m_count--;
}

bool Projectile::canDieIndependently() const
{
	return true;
}

bool Projectile::canDieFromFlame() const
{
	return false;
}

bool Projectile::isAnimate() const
{
	return false;
}

int Projectile::getCount() const
{
	return m_count;
}

Flame::Flame(StudentWorld * sWorld, double startX, double startY, Direction d)
	: Projectile(sWorld, IID_FLAME, startX, startY, d)
{
}

void Flame::doSomething()
{
	if (!isAlive())
		return;

	getWorld()->killAllKillableActors(getX(), getY(), true);

	decreaseCount();
	if (getCount() == 0)
		kill();
}

Vomit::Vomit(StudentWorld * sWorld, double startX, double startY, Direction d)
	: Projectile(sWorld, IID_VOMIT, startX, startY, d)
{
}

void Vomit::doSomething()
{
	if (!isAlive())
		return;

	getWorld()->infectAllInfectableActors(getX(), getY());

	decreaseCount();
	if (getCount() == 0)
		kill();
}

Human::Human(StudentWorld* sWorld, const int IID, double startX, double startY)
	: Actor(sWorld, IID, startX, startY, right, 0)
{
	m_paralyzed = true;
}

bool Human::turnBasedParalysis()
{
	if (m_paralyzed)
	{
		m_paralyzed = false;
		return true;;
	}
	else
	{
		m_paralyzed = true;
		return false;
	}
}

bool Human::canMove() const
{
	return true;
}

bool Human::movingActorsCanPass() const
{
	return false;
}

bool Human::okToMove(double newX, double newY) const
{
	return !getWorld()->humanOccupied(newX, newY, getX(), getY());
}

bool Human::okToMoveDir(int numSteps, Direction d)
{
	switch (d)
	{
	case up:
		return !getWorld()->humanOccupied(getX(), getY() + numSteps, getX(), getY());
		break;
	case down:
		return !getWorld()->humanOccupied(getX(), getY() - numSteps, getX(), getY());
		break;
	case left:
		return !getWorld()->humanOccupied(getX() - numSteps, getY(), getX(), getY());
		break;
	case right:
		return !getWorld()->humanOccupied(getX() + numSteps, getY(), getX(), getY());
		break;
	default:
		break;
	}
}

void Human::follow(int numSteps, Direction d)
{
	switch (d)
	{
	case up:
		setDirection(up);
		moveTo(getX(), getY() + numSteps);
		break;
	case down:
		setDirection(down);
		moveTo(getX(), getY() - numSteps);
		break;
	case left:
		setDirection(left);
		moveTo(getX() - numSteps, getY());
		break;
	case right:
		setDirection(right);
		moveTo(getX() + numSteps, getY());
		break;
	default:
		break;
	}
}

Direction Human::followDirection(Actor * closest)
{
	if (getX() == closest->getX())
	{
		if (getY() > closest->getY())
			return down;
		else
			return up;
	}
	else if (getY() == closest->getY())
	{
		if (getX() > closest->getX())
			return left;
		else
			return right;
	}
	else
	{
		bool higherUp = getY() < closest->getY();
		bool higherRight = getX() < closest->getX();
		int i = randInt(0, 1);

		//ZOMBIES ONLY - doesn't matter if they hit a wall or not, just needs a direction
		if (!canFinish())
		{
			if (i == 0)
				return (higherUp ? up : down);
			else
				return (higherRight ? right : left);
		}
		//CITIZENS ONLY - since they need to move 
		else
		{
			if (i == 0) //chooses to go up and down
			{
				if (higherUp && okToMove(getX(), getY() + 2))
					return up;
				else if (!higherUp && okToMove(getX(), getY() - 2))
					return down;
				else if (higherRight && okToMove(getX() + 2, getY()))
					return right;
				else if (okToMove(getX() - 2, getY()))
					return left;
			}
			else //chooses to go right and left
			{
				if (higherRight && okToMove(getX() + 2, getY()))
					return right;
				else if (!higherRight && okToMove(getX() - 2, getY()))
					return left;
				else if (higherUp && okToMove(getX(), getY() + 2))
					return up;
				else if (okToMove(getX(), getY() - 2))
					return down;
			}
		}
	}
}

Citizen::Citizen(StudentWorld * sWorld, double startX, double startY)
	: LiveHuman(sWorld, IID_CITIZEN, startX, startY)
{

}

void Citizen::doSomething()
{
	if (!isAlive())
	{
		return;
	}

	if (isInfected())
	{
		if (ifInfectedProcess())
		{
			getWorld()->playSound(SOUND_ZOMBIE_BORN);
			kill();

			getWorld()->increaseScore(-1000);
			int whichZombie = randInt(1, 10);
			if (whichZombie <= 7)
				getWorld()->newZombie(getX(), getY(), false);
			else
				getWorld()->newZombie(getX(), getY(), true);
			getWorld()->incrementZombies();
			getWorld()->decrementCitizens();
			return;
		}
	}

	if (turnBasedParalysis())
		return;

	double dist_p = getWorld()->distanceToPenelope(getX(), getY());
	double dist_z = 99999999999.0;
	if (getWorld()->numZombies() > 0)
		dist_z = getWorld()->closestZombie(getX(), getY());

	Direction idealD = followDirection(getWorld()->pointertoMC());
	if (dist_p <= 80.0 && dist_p < dist_z && okToMoveDir(2, idealD))
	{
		follow(2, idealD);
		return;
	}
	else if (dist_z <= 80.0 && dist_z < dist_p)
	{
		bool move_up = !getWorld()->humanOccupied(getX(), getY()+2, getX(), getY());
		bool move_down = !getWorld()->humanOccupied(getX(), getY()-2, getX(), getY());
		bool move_left = !getWorld()->humanOccupied(getX()-2, getY(), getX(), getY());
		bool move_right = !getWorld()->humanOccupied(getX()+2, getY(), getX(), getY());

		double d_up, d_down, d_left, d_right;
		d_up = getWorld()->closestZombie(getX(), getY() + 2);
		d_down = getWorld()->closestZombie(getX(), getY() - 2);
		d_left = getWorld()->closestZombie(getX() - 2, getY());
		d_right = getWorld()->closestZombie(getX() + 2, getY());

		if (d_up > dist_z && d_down > dist_z&& d_left > dist_z&& d_right > dist_z)
			return;

		if (move_up && (d_up > dist_z) &&
			((move_down && d_up >= d_down) || !move_down) &&
			((move_left && d_up >= d_left) || !move_left) &&
			((move_right&& d_up >= d_right) || !move_right))
		{
			setDirection(up);
			moveTo(getX(), getY() + 2);
			return;
		}
		else if (move_down && (d_down > dist_z) &&
			((move_up && d_down >= d_up) || !move_up) &&
			((move_left && d_down >= d_left) || !move_left) &&
			((move_right && d_down >= d_right) || !move_right))
		{
			setDirection(down);
			moveTo(getX(), getY() - 2);
			return;
		}
		else if (move_left && (d_left > dist_z) &&
			((move_up && d_left >= d_up) || !move_up) &&
			((move_down && d_left >= d_down) || !move_down) &&
			((move_right && d_left >= d_right) || !move_right))
		{
			moveTo(getX() - 2, getY());
			setDirection(left);
			return;
		}
		else if (move_right && (d_right > dist_z) &&
			((move_up && d_right >= d_up) || !move_up) &&
			((move_down && d_right >= d_down) || !move_down) &&
			((move_left && d_right >= d_left) || !move_left))
		{
			setDirection(right);
			moveTo(getX() + 2, getY());
			return;
		}
		else
			return;
	}
}

void Citizen::react()
{
	getWorld()->playSound(SOUND_CITIZEN_DIE);
	getWorld()->decrementCitizens();
	getWorld()->increaseScore(-1000);
}

Zombie::Zombie(StudentWorld * sWorld, const int imageID, double startX, double startY)
	: Human(sWorld, imageID, startX, startY)
{
	m_movementPlan = 0;
}

bool Zombie::defaultVomit()
{
	bool hasVomited = false;
	Direction d = getDirection();
	int randomNum = randInt(1, 3);
	switch (d)
	{
	case up:
		if (getWorld()->closestRealHuman(getX(), getY() + SPRITE_HEIGHT) <= 10.0)
		{
			if (randomNum == 1)
			{
				getWorld()->newProjectile(0, getX(), getY() + SPRITE_HEIGHT, up);
				hasVomited = true;
			}
		}
		break;
	case down:
		if (getWorld()->closestRealHuman(getX(), getY() - SPRITE_HEIGHT) <= 10.0)
		{
			if (randomNum == 1)
			{
				getWorld()->newProjectile(0, getX(), getY() - SPRITE_HEIGHT, down);
				hasVomited = true;
			}
		}
			break;
	case left:
		if (getWorld()->closestRealHuman(getX() - SPRITE_WIDTH, getY()) <= 10.0)
		{
			if (randomNum == 1)
			{
				getWorld()->newProjectile(0, getX() - SPRITE_WIDTH, getY(), left);
				hasVomited = true;
			}
		}
		break;
	case right:
		if (getWorld()->closestRealHuman(getX() + SPRITE_WIDTH, getY()) <= 10.0)
		{
			if (randomNum == 1)
			{
				getWorld()->newProjectile(0, getX() + SPRITE_WIDTH, getY(), right);
				hasVomited = true;
			}
		}
		break;
	default:
		break;
	}

	return hasVomited;
}

void Zombie::moveAlongRandomPlan()
{
	if (m_movementPlan== 0)
	{
		m_movementPlan = randInt(3, 10);
		setDirection(randInt(0, 3) * 90);
	}

	switch (getDirection())
	{
	case up:
		if (!(getWorld()->humanOccupied(getX(), getY() + 1, getX(), getY())))
		{
			moveTo(getX(), getY() + 1);
			m_movementPlan--;
		}
		else
			m_movementPlan = 0;
		break;
	case down:
		if (!(getWorld()->humanOccupied(getX(), getY() - 1, getX(), getY())))
		{
			moveTo(getX(), getY() - 1);
			m_movementPlan--;
		}
		else
			m_movementPlan = 0;
		break;
	case left:
		if (!(getWorld()->humanOccupied(getX() - 1, getY(), getX(), getY())))
		{
			moveTo(getX() - 1, getY());
			m_movementPlan--;
		}
		else
			m_movementPlan = 0;
		break;
	case right:
		if (!(getWorld()->humanOccupied(getX() + 1, getY(), getX(), getY())))
		{
			moveTo(getX() + 1, getY());
			m_movementPlan--;
		}
		else
			m_movementPlan = 0;
		break;
	default:
		break;
	}
}

void Zombie::moveAlongFollowPlan(Actor* closest)
{	
	Direction d = followDirection(closest);
	setDirection(d);

	switch (d)
	{
	case up:
		if (!getWorld()->humanOccupied(getX(), getY() + 1, getX(), getY()))
			moveTo(getX(), getY() + 1);
		break;
	case down:
		if (!getWorld()->humanOccupied(getX(), getY() - 1, getX(), getY()))
			moveTo(getX(), getY() - 1);
		break;
	case left:
		if (!getWorld()->humanOccupied(getX() - 1, getY(), getX(), getY()))
			moveTo(getX() - 1, getY());
		break;
	case right:
		if (!getWorld()->humanOccupied(getX() + 1, getY(), getX(), getY()))
			moveTo(getX() + 1, getY());
		break;
	default:
		break;
	}
}

DumbZombie::DumbZombie(StudentWorld * sWorld, double startX, double startY)
	: Zombie(sWorld, IID_ZOMBIE, startX, startY)
{

}

void DumbZombie::react()
{
	getWorld()->increaseScore(1000);
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	getWorld()->decrementZombies();
	int vacc = randInt(1, 10);
	if (vacc == 1)
	{
		int vDir = randInt(1, 4);
		switch (vDir)
		{
		case 1:
			if (!getWorld()->occupied(getX() + SPRITE_WIDTH, getY(), false))
				getWorld()->newObject(getX() + SPRITE_WIDTH, getY(), 'v');
			break;
		case 2:
			if (!getWorld()->occupied(getX() - SPRITE_WIDTH, getY(), false))
				getWorld()->newObject(getX() - SPRITE_WIDTH, getY(), 'v');
			break;
		case 3:
			if (!getWorld()->occupied(getX(), getY() - SPRITE_HEIGHT, false))
				getWorld()->newObject(getX(), getY() - SPRITE_HEIGHT, 'v');
			break;
		case 4:
			if (!getWorld()->occupied(getX(), getY() + SPRITE_HEIGHT, false))
				getWorld()->newObject(getX(), getY() + SPRITE_HEIGHT, 'v');
			break;
		}
	}
}

void DumbZombie::doSomething()
{
	if (!isAlive())
		return;

	if (turnBasedParalysis())
		return;

	if (defaultVomit()) 
		return;

	moveAlongRandomPlan();
}

SmartZombie::SmartZombie(StudentWorld * sWorld, double startX, double startY)
	:Zombie(sWorld, IID_ZOMBIE, startX, startY)
{

}

void SmartZombie::react()
{
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	getWorld()->decrementZombies();
	getWorld()->increaseScore(2000);
}

void SmartZombie::doSomething()
{
	if (!isAlive())
		return;

	if (turnBasedParalysis())
		return;

	if (defaultVomit())
		return;

	Actor* theclosestOne = getWorld()->closestRealHumanPointer(getX(), getY());
	if (getWorld()->distanceBetween(getX(), getY(), theclosestOne->getX(), theclosestOne->getY()) <= 80.0) 
		moveAlongFollowPlan(theclosestOne);
	else
		moveAlongRandomPlan();
}

LiveHuman::LiveHuman(StudentWorld * sWorld, const int IID, double startX, double startY)
	:Human(sWorld, IID, startX, startY)
{
	m_infectionStatus = false;
	m_infectionCount = 0;
}

void LiveHuman::infect()
{
	if (m_infectionStatus)
		return;
	else
	{
		m_infectionStatus = true;
		getWorld()->playSound(SOUND_CITIZEN_INFECTED);
	}
}

int LiveHuman::infectionNum() const
{
	return m_infectionCount;
}

void LiveHuman::resetInfectionStatus()
{
	m_infectionStatus = false;
	m_infectionCount = 0;
}

bool LiveHuman::ifInfectedProcess()
{
	if (m_infectionCount >= 500)
		return true;
	else
		m_infectionCount++;
	return false;

}

bool LiveHuman::canFinish() const
{
	return true;
}

bool LiveHuman::isInfected() const
{
	return m_infectionStatus;
}


Player::Player(StudentWorld* sWorld, double startX, double startY)
	: LiveHuman(sWorld, IID_PLAYER, startX, startY)
{

}

void Player::doSomething()
{
	if (!isAlive())
		return;

	if (isInfected())
	{
		if (ifInfectedProcess())
		{
			kill();
			getWorld()->decLives();
			getWorld()->playSound(SOUND_PLAYER_DIE);
			return;
		}
		else
		{
			getWorld()->updateMCInfectionStatus(infectionNum());
		}
	}
	else
	{
		getWorld()->updateMCInfectionStatus(0);
	}

	int value = 0;

	if (getWorld()->getKey(value))
	{
		switch (value)
		{
		case KEY_PRESS_RIGHT:
			setDirection(static_cast<Direction>(right));
			if(okToMove(getX()+4, getY()))
				moveTo(getX() + 4, getY());
			break;
		case KEY_PRESS_LEFT:
			setDirection(static_cast<Direction>(left));
			if (okToMove(getX() - 4, getY()))
				moveTo(getX() - 4, getY());
			break;
		case KEY_PRESS_UP:
			setDirection(static_cast<Direction>(up));
			if (okToMove(getX(), getY() + 4))
				moveTo(getX(), getY() + 4);
			break;
		case KEY_PRESS_DOWN:
			setDirection(static_cast<Direction>(down));
			if (okToMove(getX(), getY() - 4))
				moveTo(getX(), getY() - 4);
			break;

		case KEY_PRESS_SPACE:
			if (getWorld()->numFlames() <= 0)
				break;
			getWorld()->playSound(SOUND_PLAYER_FIRE);
			switch (getDirection())
			{
			case up:
				for (int i = 0; i <= 2; i++)
				{
					if (getWorld()->occupied(getX(), getY() + SPRITE_HEIGHT * (i + 1), false))
						break;
					getWorld()->newProjectile(1, getX(), SPRITE_HEIGHT*(i + 1) + getY(), up);
				}
				getWorld()->updateItems('F');
				break;
			case down:
				for (int i = 0; i <= 2; i++)
				{
					if (getWorld()->occupied(getX(), getY() - SPRITE_HEIGHT * (i + 1), false))
						break;
					getWorld()->newProjectile(1, getX(), -SPRITE_HEIGHT*(i + 1) + getY(), down);
				}
				getWorld()->updateItems('F');
				break;
			case left:
				for (int i = 0; i <= 2; i++)
				{
					if (getWorld()->occupied(getX() - SPRITE_HEIGHT * (i + 1), getY(), false))
						break;
					getWorld()->newProjectile(1, getX() - SPRITE_HEIGHT*(i + 1), getY(), left);
				}
				getWorld()->updateItems('F');
				break;
			case right:
				for (int i = 0; i <= 2; i++)
				{
					if (getWorld()->occupied(getX() + SPRITE_HEIGHT * (i + 1), getY(), false))
						break;
					getWorld()->newProjectile(1, getX() + SPRITE_HEIGHT * (i + 1), getY(), right);
				}
				getWorld()->updateItems('F');
				break;
			default:
				break;
			}
			break;
		case KEY_PRESS_TAB:
			if (getWorld()->numMines() > 0)
			{
				getWorld()->newObject(getX(), getY(), 'l');
				getWorld()->updateItems('M');
			}
			break;
		case KEY_PRESS_ENTER:
			if (getWorld()->numVacc() > 0)
			{
				resetInfectionStatus();
				getWorld()->updateItems('V');
			}
			break;
		default:
			break;

		}
	}
}

bool Player::okToMove(double newX, double newY) const
{
	return !(getWorld()->occupied(newX, newY, true));
}
