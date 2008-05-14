//@(#) Wrapper for curl
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id: url_easy.hpp 31 2008-05-14 18:39:24Z wingorodov $

#ifndef _URL_EASY_HPP
#define _URL_EASY_HPP 1

#include <curl/curl.h>

#include <iron.h>

#include <memchunk.h>

using namespace std;
using namespace cxx;
using namespace env;

namespace url {

///\class easy
///\brief Wrapper for cURL

class easy
{
private:
	CURL *curl;
	CURLcode res;
	mem::chunk resp;

public:

///\brief Create
easy (
	const std::string& req_ ///\param req_ Request string
) {
	init (req_);
}

///\brief Destroy
~easy () { clear(); }

///\brief Init this
void init (
	const std::string& req_ ///\param req_ Request string
) {
	curl = ::curl_easy_init ();
	if (!curl) throw runtime_error ("cURL init fail");

	::curl_easy_setopt (curl, CURLOPT_TIMEOUT, 10); 
	::curl_easy_setopt (curl, CURLOPT_SSL_VERIFYPEER, false);
	::curl_easy_setopt (curl, CURLOPT_SSL_VERIFYHOST, false);
  	::curl_easy_setopt (curl, CURLOPT_USERAGENT, "RoboTrade/1.0");
  	::curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, resp.get_write_func() );

	setURL (req_);
}

///\brief Set new URL
void setURL (
	const std::string& req_ ///\param req_ URL for request
) {
	::curl_easy_setopt (curl, CURLOPT_URL, req_.c_str()); 
}

///\brief Clean up of cURL
void clear ()
{
	::curl_easy_cleanup (curl);
   	curl = 0;
}

///\brief Get url
///\return False if error
bool request ()
{
	resp.clear();
	res = ::curl_easy_perform (curl);
	if (res == CURLE_OK) return true;
	//else
		logs << error <<  ::curl_easy_strerror (res) << endl;
		return false;
}

///\brief Get data of response
///\return A string with response
const std::string& str () const { return resp.str(); }

}; //.class easy

} //::url

#endif //_URL_EASY_HPP
