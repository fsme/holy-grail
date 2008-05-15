//@(#) Global HTTP request
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _RE_QUEST_H
#define _RE_QUEST_H 1

#include <boost/thread/mutex.hpp> 

#include <url_easy.hpp> 
#include <memparser.hpp> 
#include <synchron.h> 

using namespace std;

namespace re {

///\class quest
///\brief Make HTTP request to server
class quest
{
public:

///\brief Get server time
static
void get_server_time ()
{
try {
	HttpGet.setURL (APIHost+GetTime);
	while ( !HttpGet.request ())
	{
		logs << error << "Server is unavailable" << endl;
		::sleep (3);
		continue;
	}

	mem::parser serv_time ( HttpGet.str().data(), HttpGet.str().size());
	syn::chron::server_time (\
					 (const char*)serv_time.root_node->children->content
	);

} catch (const std::exception& e) {
		logs << error << "GetTime error: " << e.what() << endl;
}

}

///\brief Get response
static
mem::parser* http_get (
	const std::string& url_ = "" ///\param url_ URL for request
) {
	static boost::mutex	mut; ///< Thread safe

	boost::mutex::scoped_lock lock (mut);

	if (url_.size() > 8) HttpGet.setURL (url_);

	if ( !HttpGet.request())
		throw runtime_error (url_ + ": HTTP request error");

	return new mem::parser ( HttpGet.str().data(), HttpGet.str().size() );
}

private:
static url::easy HttpGet;

static const std::string APIHost;
static const std::string GetTime;

}; //.quest

} //::re

#endif //!_RE_QUEST_H
