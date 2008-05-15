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

///\brief Convert date from quotes.log and diff with realtime
static
void
	ize ( std::string& datetime ///\param datetime Date-time from quotes.log
);

///\brief Synchronize with server time
static
void
	server_time (const std::string& res_///\param res_ HTTP-response of GetTime
);

};

} //::syn

#endif //!_SYN_CHRON_H
