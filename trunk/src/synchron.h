//@(#) Synchronize clock with server
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _SYN_CHRON_H
#define _SYN_CHRON_H 1

#include <ctime> 
#include <string> 

using namespace std;

namespace syn {

///\class chron
///\brief Synchronize clock with server, fixup to realtime
class chron
{
public:
///\brief Create
chron () : _lacuna (0x7fffffff) {}

///\brief Convert date from quotes.log and diff with realtime
void
	quote_time ( std::string& date_ ///\param datetime Date as string
);

///\brief Synchronize with server time
void
	server_time (
		  const std::string& res_///\param res_ HTTP-response of GetTime
		, time_t timer_///\param timer_ Timestamp at request
);

///\brief Different from realtime to quotes time
int32_t
	realtime ()
{
	return (_quote_time - ::time (NULL) + _diff);
}

///\brief Check out break of time
int32_t lacuna () { return _lacuna; }

private:

	time_t	_diff; ///< Different from local time to quotes time
	time_t	_quote_time; ///< Timestamp from quotes log
	int32_t _lacuna; ///< Break of the time more then 300 sec.


///\brief Date to UNIX time
///\return UNIX timestamp (second from 01.01.1970)
time_t
	date_to_unixtime ( std::string& date_///\param date_ Date as string YEAR-MONTH-DAY{T| }HOUR:MIN:SEC{-...}
);

///\brief Synchronized clock with server
void
	sync ( time_t diff_///\param srvtime_ Local at request minus server's time
) {
	_diff = diff_;

cout << " DIFF=" << _diff << endl;
}

};

///\class init
///\brief Static single Clock

struct clo
{
	static chron& ck ()
	{
		static chron Clock;
		return Clock;
	}
};

} //::syn

#endif //!_SYN_CHRON_H
