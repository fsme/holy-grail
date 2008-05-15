//@(#) Memory for save rate and calculate delta
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _FX_DELTA_RATE_H
#define _FX_DELTA_RATE_H 1

#include <math.h>

#include <factor.h>

namespace fx {

///\class delta_rate
///\brief Rate and delta by last value
struct delta_rate
{
	float rate; ///< Rate
	int32_t delta; ///< Avarage rate

	///\brief Create
	delta_rate () : rate (0.0), delta (0) {}

	///\brief Create with args
	delta_rate (
		  const float rate_ ///\param rate_
		, const int32_t delta_ ///\param delta_
	)
		: rate (rate_)
		, delta (delta_)
	{}

	///\brief Recomp at new rate
	void rehash (
		    const float rate_ ///\param rate_ Current rate of CCY pair
	) {
		delta=::lroundf ((rate_ * multi::factor()) - (rate * multi::factor()));
		rate = rate_;
	}
};

}//::fx
 
#endif //!_FX_DELTA_RATE_H 
