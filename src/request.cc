//@(#) Global HTTP request (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <fstream> 

#include <request.h> 

using namespace std;

namespace re {

const string query::APIHost ("https://api.efxnow.com");
const string query::Demo ("/DEMOWebServices2.8/Service.asmx");
const string query::GetTime ("/GetTime?");
const string query::Echo ("/Echo?Message=");
const string query::Deal ("/DealRequestAtBest?UserID=");

const string query::PWD("&PWD=");
const string query::Pair("&Pair=");
const string query::Buy ("&BuySell=B");
const string query::Sell("&BuySell=S");
const string query::Amount("&Amount=");

const string query::GetPositionBlotter("/GetPositionBlotter?Key=");
const string query::GetMarginBlotter("/GetMarginBlotter?Key=");

//
mem::parser*
	query::request ( const std::string& url_ )
{
	static boost::mutex	mut; ///< Thread safe

	boost::mutex::scoped_lock lock (mut);

	if (url_.size() > 8) HttpGet.setURL (url_);

	if ( !HttpGet.request())
		throw runtime_error (url_ + ": HTTP request error");

	return new mem::parser ( HttpGet.str().data(), HttpGet.str().size() );
}

//
void flush_response ( const string comment_, const string data_ )
{
	ofstream bakfile ( iron ("fx_http_response_log").c_str(), ios::app);
	if ( !bakfile.is_open ())
	{
		if (logs << error)
			logs << "HTTP responses log is unavailable" << endl;
		return;
	}
	bakfile << comment_ << endl << data_ << endl;
}

} //::re
