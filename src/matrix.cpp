/**
 * \addtogroup lib
 * \{
 *
 * @file matrix.cpp
 *
 * Implementation of {@link matrix.hpp}
 *
 * @date 18.11.2012
 * @author Moritz Nisblé moritz.nisble@gmx.de
 */

#include "matrix.hpp"

#include <algorithm>

namespace msm
{

#define SIGNAL_GAMESTATUSCHANGED(matrix, status) do{ \
		for (std::list<MatrixObserver*>::const_iterator it = observers.begin(); it != observers.end(); ++it) { \
			(*it)->onGameStatusChanged(matrix, status); } \
		} while(0)

#define SIGNAL_REMAININGBOMBSCHANGED(matrix, bombs) do{ \
		for (std::list<MatrixObserver*>::const_iterator it = observers.begin(); it != observers.end(); ++it) { \
			(*it)->onRemainingBombsChanged(matrix, bombs); } \
		} while(0)

#define SIGNAL_FIELDSTATUSCHANGED(matrix, field, status) do{ \
		for (std::list<MatrixObserver*>::const_iterator it = observers.begin(); it != observers.end(); ++it) { \
			(*it)->onFieldStatusChanged(matrix, field, status); } \
		} while(0)

#define SIGNAL_FIELDDELETE(matrix, field) do{ \
		for (std::list<MatrixObserver*>::const_iterator it = observers.begin(); it != observers.end(); ++it) { \
			(*it)->onFieldDelete(matrix, field); } \
		} while(0)

namespace
{
char const* GS_NAMES[] =
{ "READY", "RUNNING", "WON", "LOST" };
}

char const* toString(GAMESTATUS gs)
{
	if (gs <= GS_LOST)
		return GS_NAMES[gs];
	else
		return "Index out of range!";
}

Dimensions::Dimensions(uint16_t x, uint16_t y, uint16_t bombs) :
		x(0), y(0), bombs(0)
{
	setX(x);
	setY(y);
	setBombs(bombs);
}

void Dimensions::setBombs(uint16_t _bombs)
{
	if (_bombs > (x * y))
		bombs = x * y;
	else
		bombs = _bombs;
}

void Dimensions::setX(uint16_t _x)
{
	x = _x;
	setBombs(bombs);
}

void Dimensions::setY(uint16_t _y)
{
	y = _y;
	setBombs(bombs);
}

struct Matrix::Impl: public FieldObserver
{
	Impl(Matrix* backRef) :
			backRef(backRef), matrix(0), status(GS_READY)
	{
	}

	Matrix* backRef;

	std::list<MatrixObserver*> observers;

	Dimensions dim;

	Field*** matrix;

	GAMESTATUS status;

	uint16_t unhidden;
	uint16_t marked;
	uint16_t queried;

	void deleteMatrix();

	void onFieldStatusChanged(Field const&, FIELDSTATUS);
	void onFieldDelete(Field const&);
};

Matrix::Matrix() :
		pImpl(new Matrix::Impl(this))
{
	reset(Dimensions());
}

Matrix::Matrix(Dimensions const& dimensions) :
		pImpl(new Matrix::Impl(this))
{
	reset(dimensions);
}

Matrix::~Matrix()
{
	pImpl->deleteMatrix();
	delete pImpl;
}

void Matrix::addObserver(MatrixObserver* o)
{
	if (std::find_if(pImpl->observers.begin(), pImpl->observers.end(), compare_address<MatrixObserver>(o))
			== pImpl->observers.end())
		pImpl->observers.push_back(o);
}

void Matrix::removeObserver(MatrixObserver* o)
{
	std::remove_if(pImpl->observers.begin(), pImpl->observers.end(), compare_address<MatrixObserver>(o));
}

Dimensions const& Matrix::getDimensions() const
{
	return pImpl->dim;
}

void Matrix::reset()
{
	reset(pImpl->dim);
}

void Matrix::reset(Dimensions const& dimensions)
{
	pImpl->deleteMatrix();

	pImpl->dim = dimensions;

	pImpl->status = GS_READY;

	pImpl->unhidden = 0;
	pImpl->marked = 0;
	pImpl->queried = 0;

	uint16_t dimX = pImpl->dim.getX();
	uint16_t dimY = pImpl->dim.getY();

	//Create help matrix for bomb positioning
	uint8_t** bombs = new uint8_t*[dimX];
	for (uint16_t x = 0; x < dimX; x++)
		bombs[x] = new uint8_t[dimY];

	//Fill with 0
	for (uint16_t y = 0; y < dimY; y++)
	{
		for (uint16_t x = 0; x < dimX; x++)
			bombs[x][y] = 0;
	}

	//Produce ones on bomb positions
	srand(time(0));
	uint32_t position = 0;
	for (uint16_t b = 0; b < pImpl->dim.getBombs(); b++)
	{
		position = ((rand() % (dimX * dimY)));

		//Is already bomb? Try again!
		if (bombs[(position % dimX)][position / dimX])
			b--;
		else
		{
			//Place bomb
			bombs[(position % dimX)][position / dimX] = 1;
		}
	}

	//Create matrix
	pImpl->matrix = new Field**[dimX];
	for (uint16_t x = 0; x < dimX; x++)
		pImpl->matrix[x] = new Field*[dimY];

	//Create bombs or normal fields
	for (uint16_t y = 0; y < dimY; y++)
	{
		for (uint16_t x = 0; x < dimX; x++)
		{
			(bombs[x][y] == 1) ? pImpl->matrix[x][y] = new Bomb(x, y) : pImpl->matrix[x][y] = new Field(x, y);

			pImpl->matrix[x][y]->addObserver(pImpl);
		}
	}

	//Set links to neighbours
	for (uint16_t y = 0; y < dimY; y++)
	{
		for (uint16_t x = 0; x < dimX; x++)
		{
			if (x - 1 >= 0 && y - 1 >= 0)
				pImpl->matrix[x][y]->addNeighbour(pImpl->matrix[x - 1][y - 1]);
			if (y - 1 >= 0)
				pImpl->matrix[x][y]->addNeighbour(pImpl->matrix[x][y - 1]);
			if (x + 1 < dimX && y - 1 >= 0)
				pImpl->matrix[x][y]->addNeighbour(pImpl->matrix[x + 1][y - 1]);
			if (x + 1 < dimX)
				pImpl->matrix[x][y]->addNeighbour(pImpl->matrix[x + 1][y]);
			if (x + 1 < dimX && y + 1 < dimY)
				pImpl->matrix[x][y]->addNeighbour(pImpl->matrix[x + 1][y + 1]);
			if (y + 1 < dimY)
				pImpl->matrix[x][y]->addNeighbour(pImpl->matrix[x][y + 1]);
			if (x - 1 >= 0 && y + 1 < dimY)
				pImpl->matrix[x][y]->addNeighbour(pImpl->matrix[x - 1][y + 1]);
			if (x - 1 >= 0)
				pImpl->matrix[x][y]->addNeighbour(pImpl->matrix[x - 1][y]);
		}
	}

	//Increment counters for adjacent bombs
	for (uint16_t y = 0; y < dimY; y++)
	{
		for (uint16_t x = 0; x < dimX; x++)
			pImpl->matrix[x][y]->informNeighbours();
	}

	//Delete the help matrix
	for (uint16_t x = 0; x < dimX; x++)
		delete[] bombs[x];
	delete[] bombs;

	for (std::list<MatrixObserver*>::const_iterator it = pImpl->observers.begin(); it != pImpl->observers.end(); ++it)
	{
		(*it)->onGameStatusChanged(*this, pImpl->status);
	}
#if BOOST_SIGNALS
	signalGameStatusChanged(*this, pImpl->status);
#endif
}

GAMESTATUS Matrix::getStatus() const
{
	return pImpl->status;
}

int32_t Matrix::getRemainingBombs() const
{
	return pImpl->dim.getBombs() - pImpl->marked;
}

Proxy Matrix::operator[](uint16_t x) const throw (IndexOutOfBoundsException)
{
	if (x >= pImpl->dim.getX())
		throw IndexOutOfBoundsException(x, pImpl->dim.getX(), 'X');
	else
		return Proxy(pImpl->matrix[x], pImpl->dim.getY());
}

void Matrix::Impl::deleteMatrix()
{
	//Free up all resources
	if (matrix != 0)
	{
		for (uint16_t y = 0; y < dim.getY(); y++)
		{
			for (uint16_t x = 0; x < dim.getX(); x++)
			{
				delete matrix[x][y];
			}
		}
		for (uint16_t x = 0; x < dim.getX(); x++)
			delete[] matrix[x];
		delete[] matrix;
		matrix = 0;
	}
}

void Matrix::Impl::onFieldStatusChanged(Field const& field, FIELDSTATUS newStatus)
{
	GAMESTATUS old = status;

	// Log field status
	switch (newStatus)
	{
	case FS_HIDDEN:
		// Switch from queried to hidden
		--queried;
		break;
	case FS_UNHIDDEN:
		++unhidden;
		break;
	case FS_MARKED:
		++marked;
		break;
	case FS_QUERIED:
		// Switch from marked to queried
		--marked;
		++queried;
		break;
	case FS_BOMB:
		status = GS_LOST;
		break;
	default:
		break;
	}

	if (status != GS_LOST)
	{
		if (unhidden == ((dim.getX() * dim.getY()) - dim.getBombs()) && marked == dim.getBombs())
			status = GS_WON;
		else
			status = GS_RUNNING;
	}

	SIGNAL_FIELDSTATUSCHANGED(*backRef, field, newStatus);
#if BOOST_SIGNALS
	backRef->signalFieldStatusChanged(field, newStatus);
#endif

	if (FS_MARKED == newStatus || FS_QUERIED == newStatus)
	{
		SIGNAL_REMAININGBOMBSCHANGED(*backRef, dim.getBombs() - marked);
#if BOOST_SIGNALS
		backRef->signalRemainingBombsChanged(*backRef, dim.getBombs() - marked);
#endif
	}

	if (status != old)
	{
		SIGNAL_GAMESTATUSCHANGED(*backRef, status);
#if BOOST_SIGNALS
		backRef->signalGameStatusChanged(*backRef, status);
#endif
	}
}

void Matrix::Impl::onFieldDelete(Field const& field)
{
	SIGNAL_FIELDDELETE(*backRef, field);
#if BOOST_SIGNALS
	backRef->signalFieldDelete(field);
#endif
}

} //namespace msm

///\}
