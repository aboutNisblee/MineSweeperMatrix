/**
 * \addtogroup lib
 * \{
 *
 * \file field.hpp
 *
 * \date 18.11.2012
 * \author Moritz Nisblé moritz.nisble@gmx.de
 */

#ifndef FIELD_H_
#define FIELD_H_

#include <stdint.h>
#include <vector>
#include <list>

#include "config.hpp"
#include "jniRef.hpp"

namespace msm
{

/** \enum msm::FIELDSTATUS
 * \brief Current status of \ref msm::Field "fields".
 * \todo Use struct and add a toString() method! */
enum FIELDSTATUS
{
	FS_HIDDEN,  //!< Hidden field.
	FS_UNHIDDEN, //!< Revealed field.
	FS_MARKED, //!< Marked field.
	FS_QUERIED,  //!< Marked field.
	FS_BOMB    //!< Bomb.
};

/**
 * Get the name of a given \ref #FIELDSTATUS "field status" constant.
 * \param status A field status.
 * \return A string literal.
 */
char const* toString(FIELDSTATUS status);

/// A position of a \ref Field "field" in a \ref Matrix "matrix".
struct Position
{
	/// Constructor.
	Position(uint16_t x, uint16_t y) :
			X(x), Y(y)
	{
	}
	/// The horizontal position.
	uint16_t X;
	/// The vertical position.
	uint16_t Y;
};

class Field;

/// Interface for Field observers
struct FieldObserver
{
	virtual ~FieldObserver()
	{
	}
	/**
	 * Signal that is emitted on change of the \ref FIELDSTATUS "status" of this field.
	 * \param field A reference to the Field.
	 * \param status The new status.
	 */
	virtual void onFieldStatusChanged(Field const& field, FIELDSTATUS status) = 0;
	/**
	 * Signal that is emitted before a field is destroyed.
	 * \param field A reference to the Field.
	 */
	virtual void onFieldDelete(Field const& field) = 0;
};

/** \class Field
 * A normal field. */
#if JNIREF
class Field: public JNIRef
#else
class Field
#endif
{
public:
	struct Impl;

	/** Constructor.
	 * \param position The position of the field in the matrix. */
	explicit Field(Position const& position);
	/** Constructor.
	 * \param x The horizontal position.
	 * \param y The vertical position. */
	Field(uint16_t x, uint16_t y);
	/// Destructor.
	virtual ~Field();

	/** Register a field observer.
	 * \param observer An Observer. */
	void addObserver(FieldObserver* observer);
	/** Remove a field observer.
	 * \param observer An Observer. */
	void removeObserver(FieldObserver* observer);

	/// Get the \ref Position "position".
	Position const& getPosition() const;
	/// Get the current \ref FIELDSTATUS "status" of the field.
	FIELDSTATUS getStatus() const;
	/// \internal Add a adjacent field into the vector for neighbours.
	void addNeighbour(Field* field);
	/// \internal Clear neighbours.
	void clearNeighbours();
	/// \internal Increment the count of adjacent bombs.
	void incAdjacentBombs();
	///Get the count of adjacent bombs.
	uint8_t getAdjacentBombs() const;
	/** Cycle through the the three states:
	 * Marked, Queried, Unmarked */
	void cycleMark();
	/** \internal Interface method that is overwritten by \ref Bomb "bombs"
	 * and used to inform neighbours about the bomb status. */
	virtual void informNeighbours();
	/** Reveal the field.
	 * - The \ref ::FIELDSTATUS "status" of the field is set to UNHIDDEN.
	 * - The signal for changed status is emitted.
	 * - If there are no adjacent bombs, all neighbors that are not already UNHIDDEN
	 * and not marked 1 and are also revealed.
	 * \return The count of adjacent bombs. */
	virtual uint8_t reveal();

protected:
	Impl* pImpl;
};

/// A bomb.
class Bomb: public Field
{
public:
	/** Constructor.
	 * \param position The position of the bomb in the matrix. */
	explicit Bomb(Position const& position);
	/** Constructor.
	 * \param x The horizontal position.
	 * \param y The vertical position. */
	Bomb(uint16_t x, uint16_t y);
	/// Destructor.
	virtual ~Bomb();
	/// Inform neighbours about the bomb status.
	void informNeighbours();
	/** Reveal the bomb.
	 * \return \ref FS_BOMB "bomb status". */
	uint8_t reveal();
};

} //namespace msm

#endif /* FIELD_H_ */

///\}
