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
	: HttpGet (APIHost+Demo+GetTime)
{
	clo::ck();
	get_server_time ();
}

///\brief Get server time
void get_server_time ()
{
try {
	HttpGet.setURL (APIHost+Demo+GetTime);
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
try {
	string http_get_buy (\
			  APIHost + Demo
			+ Deal + iron ("fx_demo_user")
			+ PWD + iron ("fx_demo_pwd")
			+ Pair + iron ("y")
			+ Buy + Amount + "100000"
		);
	auto_ptr<mem::parser> _resp ( response (http_get_buy) );
	if (logs << info)
		logs << "REAL BUY=" << _resp.get()->str() << flush;

} catch (const exception& e) {
	logs << error << "HTTP BUY ERROR: " << e.what() << endl;
	return false;
}

	return true;
}

///\brief Sell HTTP-request
bool sell ()
{
try {
	string http_get_sell (\
			  APIHost + Demo
			+ Deal + iron ("fx_demo_user")
			+ PWD + iron ("fx_demo_pwd")
			+ Pair + iron ("y")
			+ Sell + Amount + "100000"
		);
	auto_ptr<mem::parser> _resp ( response (http_get_sell) );
	if (logs << info)
		logs << "REAL SELL=" << _resp.get()->str() << flush;

} catch (const exception& e) {
	logs << error << "HTTP SELL ERROR: " << e.what() << endl;
	return false;
}

	return true;
}

///\brief Echo HTTP-request
bool echo (
		const std::string& shout_ = "CooL" ///\param shout_ Text for echo
) {
	auto_ptr<mem::parser> _resp ( response (APIHost+Demo+Echo+shout_) );
	logs << "Echo="<< (const char*)_resp.get()->root_node->children->content
		 << endl;

	return true;
}

///\brief Get position blotter as string
///\return Position blotter
std::string get_position()
{
	auto_ptr<mem::parser> _resp (\
			 response (APIHost+Demo+GetPositionBlotter+iron("fx_demo_key")) );

	return _resp.get()->str();
}

///\brief Get margin blotter as string
///\return Margin blotter
std::string get_margin()
{
	auto_ptr<mem::parser> _resp (\
			 response (APIHost+Demo+GetMarginBlotter+iron("fx_demo_key")) );

	return _resp.get()->str();
}

private:

///\brief Get response
mem::parser* response (
	const std::string& url_ = ""///\param url_ URL for request
);

	url::easy HttpGet;

static const string APIHost;
static const string Demo;
static const string GetTime;
static const string Echo;
static const string Deal;
static const string PWD;
static const string Pair;
static const string Buy;
static const string Sell;
static const string Amount;

static const string GetPositionBlotter;
static const string GetMarginBlotter;

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
