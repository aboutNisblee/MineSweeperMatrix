/**
 * \addtogroup lib
 * \{
 *
 * \file matrix.hpp
 *
 *
 * \date 18.11.2012
 * \author Moritz Nisblé moritz.nisble@gmx.de
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdint.h>

#include "config.hpp"
#include "field.hpp"
#include "tools.hpp"

#if BOOST_SIGNALS
#include <boost/signals2.hpp>
#endif

/// Namespace of the library.
namespace msm
{

/** This class holds the configuration of a Matrix.
 * X and Y is the count of the fields, not the index.
 * The setters ensures that the count of bombs is never higher than the count of fields. */
class Dimensions
{
public:
	/// Constuctor that initializes all member to zero.
	Dimensions() :
			x(0), y(0), bombs(0)
	{
	}
	/** Constuctor.
	 * \param x The horizontal count of fields.
	 * \param y The vertical count of fields.
	 * \param bombs The count of bombs.
	 */
	Dimensions(uint16_t x, uint16_t y, uint16_t bombs);
	/// Get the bomb count.
	uint16_t getBombs() const
	{
		return bombs;
	}
	/// Sets the bomb count.
	void setBombs(uint16_t bombs);
	/// Returns the count of \ref Field "fields" in the horizontal direction.
	uint16_t getX() const
	{
		return x;
	}
	/// Sets the count of the \ref Field "fields" in the horizontal direction.
	void setX(uint16_t x);
	/// Returns the count of the \ref Field "fields" in the vertical direction.
	uint16_t getY() const
	{
		return y;
	}
	/// Sets the count of the \ref Field "fields" in the vertical direction.
	void setY(uint16_t y);
private:
	uint16_t x;
	uint16_t y;
	uint16_t bombs;
};

/// The current status of the game.
enum GAMESTATUS
{
	GS_READY, //!< Status of the game before any click was made.
	GS_RUNNING, //!< Status of a running game.
	GS_WON,    //!< Status after all bombs are marked and all other fields are revealed.
	GS_LOST   //!< Status after a bomb was revealed.
};

/**
 * Get the name of a given \ref #GAMESTATUS "game status" constant.
 * \param status A game status.
 * \return A string literal.
 */
char const* toString(GAMESTATUS status);

class Matrix;

/// Interface for Matrix observers
struct MatrixObserver
{
	virtual ~MatrixObserver()
	{
	}
	/**
	 * Method that is called after the internal \ref #GAMESTATUS "status"
	 * of the matrix has changed.
	 * \param matrix A reference to the matrix.
	 * \param status The new status.
	 */
	virtual void onGameStatusChanged(Matrix const& matrix, GAMESTATUS status) = 0;
	/**
	 * Method that is called when the remaining bombs has changed
	 * because the user has marked or unmarked a Field by a call to Field.cycleMark().
	 * \note The count depends only on the fields that are marked and the initial
	 * bomb count configured in given Dimensions. Therefore this value can also be
	 * negative, when the player marked more fields than bombs are present in the game.
	 * \param matrix A reference to the matrix.
	 * \param bombs The new count of remaining bombs.
	 */
	virtual void onRemainingBombsChanged(Matrix const& matrix, int32_t bombs) = 0;
	/**
	 * Method that is called after a field has changed its \ref FIELDSTATUS "status".
	 * \note This callback is common to all Fields. You can also connect to
	 * FieldObserver.onFieldStatusChanged() to get informed by the field itself.
	 * \param matrix The Matrix.
	 * \param field The Field.
	 * \param status The new status of the field.
	 */
	virtual void onFieldStatusChanged(Matrix const& matrix, Field const& field, FIELDSTATUS status) = 0;
	/**
	 * Method that is called right before a field is deleted.
	 * \note This callback is common to all Fields. You can also connect to
	 * FieldObserver.onFieldDelete() to get informed by the field itself.
	 * \param matrix The Matrix.
	 * \param field The Field.
	 */
	virtual void onFieldDelete(Matrix const& matrix, Field const& field) = 0;
};

/**
 * This class holds the logic to build and manipulate the matrix.
 * It overloads the 2-dimensional array-operator ([][]) to provide convenient
 * access to the \ref Field "fields".
 */
class Matrix
{
public:
	struct Impl;

	/// Create empty Matrix
	Matrix();
	/// Create Matrix.
	explicit Matrix(Dimensions const& dimensions);
	/// Destructor.
	virtual ~Matrix();

	/** Register a matrix observer.
	 * \param observer An Observer. */
	void addObserver(MatrixObserver* observer);
	/** Remove a matrix observer.
	 * \param observer An Observer. */
	void removeObserver(MatrixObserver* observer);

	/// Returns the currently used \ref Dimensions.
	Dimensions const& getDimensions() const;
	/// Reset Matrix with the currently configured Dimensions.
	void reset();
	/// Reset Matrix with new Dimensions.
	void reset(Dimensions const& dimensions);
	/// Get the current \ref #GAMESTATUS "game status".
	GAMESTATUS getStatus() const;
	/// Get the remaining bomb count.
	int32_t getRemainingBombs() const;

	/** Field-access-operator for the horizontal dimension.
	 * \note This function returns a Proxy for the access to the vertical dimension.
	 * \param x The X-coordinate inside the matrix.
	 * \return A Proxy.
	 */
	Proxy operator[](uint16_t x) const throw (IndexOutOfBoundsException);

#if BOOST_SIGNALS
	/**
	 * \var signalFieldStatusChanged
	 * Signal that is emitted on change of the \ref #FIELDSTATUS "status" of a field.
	 * This is a convenient method to get called back when any field changes it's state.
	 * If you need each field to inform it's own observer, register an observer in each field
	 * instead of using this signal.
	 */

	/**
	 * \var signalFieldDelete
	 * Signal that is emitted before a field is destroyed.
	 * If you need each field to inform it's own observer, use the signals of the fields
	 * instead of this one.
	 */

	boost::signals2::signal<void(Field const&, FIELDSTATUS)> signalFieldStatusChanged;
	boost::signals2::signal<void(Field const&)> signalFieldDelete;

	/**
	 * \var signalGameStatusChanged
	 * Signal that is emitted when the \ref #GAMESTATUS "status" of the game changes.
	 */

	/**
	 * \var signalRemainingBombsChanged
	 * Signal that is emitted when the remaining bomb count changes.
	 * \note The count depends only on the fields that are marked and the initial
	 * bomb count configured in given Dimensions. Therefore this value can also be
	 * negative, when the player marked more fields than bombs are present in the game.
	 */

	boost::signals2::signal<void(Matrix const&, GAMESTATUS)> signalGameStatusChanged;
	boost::signals2::signal<void(Matrix const&, int32_t)> signalRemainingBombsChanged;
#endif

protected:
	Impl* pImpl;

private:
	/* Copy feature removed...
	 * The deep copy is too error prone
	 * and not worthwhile. */
	Matrix(Matrix const& cp);
	Matrix& operator=(Matrix const& cp);
};

} //namespace msm

#endif /* MATRIX_H_ */

///\}
