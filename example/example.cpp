/**
 * @file example.cpp
 *
 * @date 18.11.2012
 * @author Moritz Nisblé moritz.nisble@gmx.de
 */

#include <cstdlib>
#include <stdint.h>
#include <iostream>
#include <string>
#include <ctime>

#include "matrix.hpp"

void dumpMatrix(msm::Matrix& matrix)
{
	for (uint16_t iI = 0; iI < matrix.getDimensions().getY(); iI++)
	{
		for (uint16_t iJ = 0; iJ < matrix.getDimensions().getX(); iJ++)
		{
			if (matrix[iJ][iI].getStatus() == msm::FS_HIDDEN)
				std::cout << "H ";
			else if (matrix[iJ][iI].getStatus() == msm::FS_BOMB)
				std::cout << "X ";
			else if (matrix[iJ][iI].getStatus() == msm::FS_UNHIDDEN)
			{
				uint8_t adjacentBombs = matrix[iJ][iI].getAdjacentBombs();
				if (adjacentBombs > 0)
					std::cout << (uint16_t) adjacentBombs;
				else
					std::cout << " ";

				std::cout << " ";
			}
		}
		std::cout << "\n";
	}

	for (int iI = 0; iI < matrix.getDimensions().getX(); iI++)
		std::cout << "--";
	std::cout << "\n";
}

void onStateChanged(msm::Field const& f, msm::FIELDSTATUS newStatus)
{
	std::cout << "Status of X:" << f.getPosition().X << " Y:" << f.getPosition().Y << " changed to "
			<< msm::toString(newStatus) << "\n";
}

void onDelete(msm::Field const& f)
{
	std::cout << "Deleting X:" << f.getPosition().X << " Y:" << f.getPosition().Y << "\n";
}

struct MatrixHandler: public msm::MatrixObserver
{
	void onGameStatusChanged(msm::Matrix const& /* matrix */, msm::GAMESTATUS newStatus)
	{
		std::cout << "Gamestatus changed to " << msm::toString(newStatus) << "\n";
	}
	void onRemainingBombsChanged(msm::Matrix const& /* matrix */, int32_t remainingBombs)
	{
		std::cout << "Remaining bombs: " << remainingBombs << "\n";
	}
	void onFieldStatusChanged(msm::Matrix const& /* matrix */, msm::Field const& f, msm::FIELDSTATUS newStatus)
	{
		std::cout << "Status of X:" << f.getPosition().X << " Y:" << f.getPosition().Y << " changed to "
				<< msm::toString(newStatus) << "\n";
	}
	void onFieldDelete(msm::Matrix const& /* matrix */, msm::Field const& f)
	{
		std::cout << "Deleting X:" << f.getPosition().X << " Y:" << f.getPosition().Y << "\n";
	}
};

int main(int /*argc*/, char** /*argv*/)
{
	clock_t program_start = clock();

	// Create a dimension object
	msm::Dimensions dim(20, 20, 20);
	// Create a handler that is passed as observer to the matrix
	MatrixHandler handler;

	// Create an empty matrix object
	msm::Matrix* m = new msm::Matrix();
	// Add the observer
	m->addObserver(&handler);

	// Reset matrix to our dimensions
	std::cout << "Creating matrix: " << dim.getX() << "x" << dim.getY() << " with " << dim.getBombs() << " bombs\n";
	clock_t create_start = clock();
	m->reset(dim);
	clock_t create_end = clock();

	/* If boost is enabled we can add slots to get informed
	 * by matrix when a field changes its status. */
#if BOOST_SIGNALS
	m->signalFieldStatusChanged.connect(onStateChanged);
	/*m->signalFieldDelete.connect(onDelete);*/
#endif

	// Dump initial matrix
	std::cout << "Initial matrix dump\n";
	dumpMatrix(*m);

	clock_t reveal_sum = 0;
	try
	{
		for (uint16_t y = 0; y < dim.getY() && m->getStatus() <= msm::GS_RUNNING; y++)
		{
			for (uint16_t x = 0; x < dim.getX() && m->getStatus() <= msm::GS_RUNNING; x++)
			{
				std::cout << "Click X: " << x << " Y: " << y << "\n";
				clock_t reveal_start = clock();
				(*m)[x][y].reveal();
				reveal_sum += clock() - reveal_start;
				dumpMatrix(*m);
			}
		}
	} catch (std::exception const& e)
	{
		std::cout << e.what() << std::endl;
	}

	delete m;

	std::cout << "Creation time: " << ((double) (create_end - create_start) * 1000.0 / CLOCKS_PER_SEC) << "ms\n";
	std::cout << "Reveal time: " << ((double) (reveal_sum) * 1000.0 / CLOCKS_PER_SEC) << "ms\n";
	std::cout << "Program runtime: " << ((double) (clock() - program_start) * 1000.0 / CLOCKS_PER_SEC) << "ms\n";

	return EXIT_SUCCESS;
}

