//@(#) Global HTTP request (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <request.h> 

using namespace std;

namespace re {

const string query::APIHost ("https://api.efxnow.com");
const string query::GetTime ("/DEMOWebServices2.8/Service.asmx/GetTime?");
const string query::Echo ("/DEMOWebServices2.8/Service.asmx/Echo?Message=");

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
