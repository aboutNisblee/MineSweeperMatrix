/**
 * \addtogroup config
 * \{
 *
 * \file config.hpp
 *
 * \date 18.11.2012
 * \author Moritz Nisblé moritz.nisble@gmx.de
 */

#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#ifndef JNIREF
/// Enable/Disable JNI-Callbacks.
#define JNIREF 1
#endif

#ifndef BOOST_SIGNALS
/// Enable/Disable additional signals (boost::signals2 needed).
#define BOOST_SIGNALS 1
#endif

#endif /* CONFIG_HPP_ */

///\}
