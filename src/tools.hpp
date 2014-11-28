/**
 * \addtogroup tools
 * \{
 *
 * \file tools.hpp
 *
 * \date 18.11.2012
 * \author Moritz Nisblé moritz.nisble@gmx.de
 */

#ifndef SRC_TOOLS_HPP_
#define SRC_TOOLS_HPP_

#include <stdint.h>
#include <exception>
#include <sstream>

#include "field.hpp"

namespace msm
{

template<class T>
struct compare_address
{
	explicit compare_address(T const* p) :
			p(p)
	{
	}

	bool operator()(T const* rhs)
	{
		return (p == rhs);
	}

	T const* p;
};

///Exception that is throw when the Matrix is accessed at an invalid position.
class IndexOutOfBoundsException: public std::exception
{
public:
	/// Constructor.
	IndexOutOfBoundsException() throw ();
	/** Constructor.
	 * \param index The index that was illegal accessed.
	 * \param length The length of the array.
	 * \param dimension A char describing the dimension in the Matrix (e.g. 'X'). */
	IndexOutOfBoundsException(uint16_t index, uint16_t length, char dimension) throw ()
	{
		idx = index;
		len = length;
		dim = dimension;
	}
	/// Destructor.
	virtual ~IndexOutOfBoundsException() throw ()
	{
	}
	/** What message.
	 * \return A c-style string with the error message. */
	virtual const char* what() const throw ()
	{
		std::stringstream ss;
		ss << "Matrix index out of bounds. Index: " << idx << " exceeding array length of: " << len << " in dimension: "
				<< dim;
		return ss.str().c_str();
	}

	uint16_t idx;
	uint16_t len;
	char dim;
};

/// Helper class to enable overloading of the 2-dimensional array operator.
class Proxy
{
public:
	Proxy(Field** x, uint16_t maxY) :
			x(x), maxY(maxY)
	{
	}

	/** Field-access-operator for the vertical dimension.
	 * \param y The Y-coordinate inside the matrix.
	 * \return Reference to the \ref Field "field" at the given coordinates.
	 */
	Field& operator[](uint16_t y) const throw (IndexOutOfBoundsException)
	{
		if (y >= maxY)
			throw IndexOutOfBoundsException(y, maxY, 'Y');
		else
			return *x[y];
	}
private:
	Field** x;
	uint16_t maxY;
};

} // namespace msm

#endif /* SRC_TOOLS_HPP_ */

///\}
