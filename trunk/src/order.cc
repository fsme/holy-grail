//@(#) Order for deal (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id: order.cc 31 2008-05-14 18:39:24Z wingorodov $

#include <order.h>

namespace deal {

boost::mutex	order::mut;
bool			order::master = false;

const std::string order::APIDemoHost ("https://api.efxnow.com");
const std::string order::Echo("/DEMOWebServices2.8/Service.asmx/Echo?Message=");
url::easy order::DealRequest (APIDemoHost+Echo+"RoboTrade");

//
void order::lock_for_real ()
{
	boost::mutex::scoped_lock lock (mut);
	if ( is_real() && !master )
		master = true;
	else
		_that_real = false;
}

} //::deal
