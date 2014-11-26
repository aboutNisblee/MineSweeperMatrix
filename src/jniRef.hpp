/**
 * \addtogroup tools
 * \{
 *
 * \file jniRef.hpp
 *
 * This file implements a framework for callbacks from C++ into Java using JNI.\n
 * The class JNIRef can serve as a base class for objects that should be able to
 * call objects in Java.\n
 * In the JNI-File a global reference can be stored in such objects and restored
 * after the C++ object e.g. has emitted a signal.
 *
 * \date 17.02.2013
 * \author Moritz Nisblé moritz.nisble@gmx.de
 */

#ifndef JNIREF_HPP_
#define JNIREF_HPP_

namespace msm
{

/// Derive from this class when you plan to call back methods in Java.
class JNIRef
{
public:
	/**
	 * Constructor.
	 */
	JNIRef() :
			ref(0), method(0)
	{
	}
	/**
	 * Destructor.
	 */
	virtual ~JNIRef()
	{
	}

	/**
	 * Sets a pointer to a object in java where the method to call back lies.
	 * \note You have to provide a so called global reference to prevent the
	 * reference from moving around by the garbage collector. Use it like this:
	 * setJavaRef((jniObject*) env->NewGlobalRef(obj)); while obj is of type jobject
	 * when passed to JNI-function and a reference to the object where the method to call back lies.
	 * \param global_ref A global reference to the object to call back.
	 */
	void setJavaRef(void* global_ref)
	{
		ref = global_ref;
	}

	/**
	 * Getter for the Java object.
	 * This is needed to invoke the callback from JNI to Java.
	 * \return The address of the java object where the method to call back lies.
	 * Use \ref getJavaMethodID() to get the right method.
	 */
	void* getJavaRef() const
	{
		return ref;
	}

	/**
	 * We also need the ID of the method to call back.
	 * \param methodID The jmethodID of the java method to call back.
	 */
	void setJavaMethodID(void* methodID)
	{
		method = methodID;
	}

	/**
	 * Get the method ID.
	 * This is needed to invoke the callback from JNI to Java.
	 * \return The ID of the method to call back.
	 */
	void* const& getJavaMethodID() const
	{
		return method;
	}

protected:
	void* ref;
	void* method;
};

} //namespace msm

#endif /* JNIREF_HPP_ */

///\}
