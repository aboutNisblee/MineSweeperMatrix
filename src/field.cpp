/**
 * \addtogroup lib
 * \{
 *
 * \file field.cpp
 *
 * Implementation of \ref field.hpp
 *
 * \date 18.11.2012
 * \author Moritz Nisblé moritz.nisble@gmx.de
 */

#include "field.hpp"

#include <algorithm>

#include "tools.hpp"

namespace msm
{

#define SIGNAL_FIELDSTATUSCHANGED(field, status) do{ \
		for (std::list<FieldObserver*>::const_iterator it = pImpl->observers.begin(); it != pImpl->observers.end(); ++it) { \
			(*it)->onFieldStatusChanged(field, status); } \
		} while(0)
#define SIGNAL_FIELDDELETE(field) do{ \
		for (std::list<FieldObserver*>::const_iterator it = pImpl->observers.begin(); it != pImpl->observers.end(); ++it) { \
			(*it)->onFieldDelete(field); } \
		} while(0)

namespace
{
char const* FS_NAMES[] =
{ "HIDDEN", "UNHIDDEN", "MARKED", "QUERIED", "BOMB" };
}

struct Field::Impl
{
	Impl(Position p) :
			pos(p), status(FS_HIDDEN), adjacentBombs(0), neighbours(0)
	{
	}
	virtual ~Impl()
	{
	}

	Position pos;
	FIELDSTATUS status;

	uint8_t adjacentBombs;

	//TODO: Use a fixed size map or a simple array!
	std::vector<Field*> neighbours;

	std::list<FieldObserver*> observers;
};

char const* toString(FIELDSTATUS fs)
{
	if (fs <= FS_BOMB)
		return FS_NAMES[fs];
	else
		return "Index out of range!";
}

Field::Field(Position const& position) :
		pImpl(new Field::Impl(position))
{
}

Field::Field(uint16_t x, uint16_t y) :
		pImpl(new Field::Impl(Position(x, y)))
{
}

Field::~Field()
{
	SIGNAL_FIELDDELETE(*this);
	delete pImpl;
}

void Field::addObserver(FieldObserver* o)
{
	if (std::find_if(pImpl->observers.begin(), pImpl->observers.end(), compare_address<FieldObserver>(o))
			== pImpl->observers.end())
		pImpl->observers.push_back(o);
}

void Field::removeObserver(FieldObserver* o)
{
	std::remove_if(pImpl->observers.begin(), pImpl->observers.end(), compare_address<FieldObserver>(o));
}

Position const& Field::getPosition() const
{
	return pImpl->pos;
}

FIELDSTATUS Field::getStatus() const
{
	return pImpl->status;
}

void Field::addNeighbour(Field* field)
{
	pImpl->neighbours.push_back(field);
}

void Field::clearNeighbours()
{
	pImpl->neighbours.clear();
}

void Field::incAdjacentBombs()
{
	//Remember: A child can only access the protected members of its own base class instance!
	pImpl->adjacentBombs++;
}

uint8_t Field::getAdjacentBombs() const
{
	return pImpl->adjacentBombs;
}

uint8_t Field::reveal()
{
	if (pImpl->status != FS_MARKED && pImpl->status != FS_UNHIDDEN)
	{
		pImpl->status = FS_UNHIDDEN;

		SIGNAL_FIELDSTATUSCHANGED(*this, FS_UNHIDDEN);

		if (pImpl->adjacentBombs == 0)
		{
			std::vector<Field*>::iterator it;
			for (it = pImpl->neighbours.begin(); it != pImpl->neighbours.end(); it++)
			{
				(*it)->reveal();
			}
		}
	}

	return pImpl->adjacentBombs;
}

void Field::cycleMark()
{
	FIELDSTATUS old = pImpl->status;

	switch (pImpl->status)
	{
	case FS_HIDDEN:
		pImpl->status = FS_MARKED;
		break;
	case FS_MARKED:
		pImpl->status = FS_QUERIED;
		break;
	case FS_QUERIED:
		pImpl->status = FS_HIDDEN;
		break;
	default:
		break;
	}

	if (pImpl->status != old)
		SIGNAL_FIELDSTATUSCHANGED(*this, pImpl->status);
}

void Field::informNeighbours()
{
}

Bomb::Bomb(Position const& position) :
		Field(position)
{
}

Bomb::Bomb(uint16_t x, uint16_t y) :
		Field(Position(x, y))
{
}

Bomb::~Bomb()
{
}

void Bomb::informNeighbours()
{
	std::vector<Field*>::iterator it;
	for (it = pImpl->neighbours.begin(); it != pImpl->neighbours.end(); it++)
		(*it)->incAdjacentBombs();
}

uint8_t Bomb::reveal()
{
	if (pImpl->status != FS_MARKED && pImpl->status != FS_BOMB && pImpl->status != FS_UNHIDDEN)
	{
		pImpl->status = FS_BOMB;
		SIGNAL_FIELDSTATUSCHANGED(*this, FS_BOMB);
	}
	return FS_BOMB;
}

} //namespace msm

///\}
