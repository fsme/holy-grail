//@(#) Global HTTP request (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <request.h> 

using namespace std;

namespace re {

const string query::APIHost ("https://api.efxnow.com");
const string query::DemoGetTime ("/DEMOWebServices2.8/Service.asmx/GetTime?");
const string query::DemoEcho ("/DEMOWebServices2.8/Service.asmx/Echo?Message=");
const string query::DemoDeal ("/DEMOWebServices2.8/Service.asmx/DealRequestAtBest?UserID=");
const string query::PWD("&PWD=");
const string query::Pair("&Pair=");
const string query::Buy ("&BuySell=B");
const string query::Sell("&BuySell=S");
const string query::Amount("&Amount=");

//UserID=string&PWD=string&Pair=string&BuySell=string&Amount=string HTTP/1.1

//
mem::parser*
	query::response ( const std::string& url_ )
{
	static boost::mutex	mut; ///< Thread safe

	boost::mutex::scoped_lock lock (mut);

	if (url_.size() > 8) HttpGet.setURL (url_);

	if ( !HttpGet.request())
		throw runtime_error (url_ + ": HTTP request error");

	return new mem::parser ( HttpGet.str().data(), HttpGet.str().size() );
}

} //::re
