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
using namespace syn;

namespace re {

///\class query
///\brief Make HTTP request to server
class query
{
public:

///\brief Create
query ()
	: HttpGet (APIHost+GetTime)
{
	clo::ck();
	get_server_time ();
}

///\brief Get server time
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
	clo::ck().server_time (\
		  (const char*) serv_time.root_node->children->content
		, HttpGet.timer ()
	);

} catch (const std::exception& e) {
		logs << error << "GetTime error: " << e.what() << endl;
}

}

///\brief BUY HTTP-request
bool buy ()
{
	logs << " REAL BUY! ";
	return true;
}

///\brief Sell HTTP-request
bool sell ()
{
	logs << " REAL SELL! ";
	return true;
}

///\brief Echo HTTP-request
bool echo (
		const std::string& shout_ = "" ///\param shout_ Text for echo
) {
	mem::parser* _resp = response (APIHost+Echo+shout_);
	logs<< " Echo=" << (const char*)_resp->root_node->children->content << "; ";
	delete _resp;
	return true;
}

private:

///\brief Get response
mem::parser* response (
	const std::string& url_ = "" ///\param url_ URL for request
);

	url::easy HttpGet;

static const string APIHost;
static const string GetTime;
static const string Echo;

}; //.query


///\class que
///\brief Static single HTTP request 
struct que
{
	static query& st ()
	{
		static query HttpRequest;
		return HttpRequest;
	}
};

} //::re

#endif //!_RE_QUEST_H
