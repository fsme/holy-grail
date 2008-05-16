//@(#) Order for deal (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <order.h>
#include <iron.h>

using namespace syn;
using namespace std;

namespace deal {

//
bool order::lock_for_real ( const int32_t id_,  const bool lock_ )
{
	static boost::mutex	mut; ///< Lock master's flag is thread protected
	static int32_t _id = 0; ///< Identifier of order

	boost::mutex::scoped_lock lock (mut);

logs << " TRY TO LOCK Id=" << id_ << " Lock=" << boolalpha << lock_ << " COND=" << _id ;

	if ( !lock_ && _id == id_ ) ///< Unlock
	{
		_id ^= _id;
		return false;
	}

	if ( env::iron.exists ("r")
	&&  (clo::ck().realtime() > 1 || clo::ck().realtime() < -1)
	)	
	 return false;

	if ( lock_ && 0 == _id ) ///< Lock
	{
		_id = id_;
		return true;
	}

	return false;
}

} //::deal
