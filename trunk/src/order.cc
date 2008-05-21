//@(#) Order for deal (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <boost/thread/mutex.hpp>

#include <order.h>
#include <iron.h>

using namespace syn;
using namespace std;

namespace deal {

//
bool order::lock_for_real ( const bool lock_ )
{
	static boost::mutex	mut; ///< Lock master's flag is thread protected
	static int32_t _id = 0; ///< Identifier of order

	boost::mutex::scoped_lock lock (mut);

//logs<< " TRY="<< boolalpha<< lock_ << " LOCK BY="<< id() << " EXISTS="<< _id ;

	if ( !lock_ )
	{
		if ( _id == id() ) _id ^= _id; ///< Unlock
		return false;
	}

	if ( env::iron.exists ("r")
	&&  ( clo::ck().realtime() < -2 || clo::ck().realtime() > 1 )
	)	{
		if (logs << info)
			logs << "Lock refuse because realtime=" << clo::ck().realtime()
				 << endl;
		 return false;
	}

	if ( lock_ && 0 == _id ) ///< Lock
	{
		_id = id();
		return true;
	}

	return false;
}

} //::deal
