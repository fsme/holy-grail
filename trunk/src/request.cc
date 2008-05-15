//@(#) Global HTTP request (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <request.h> 

using namespace std;

namespace re {

const std::string quest::APIHost ("https://api.efxnow.com");
const std::string quest::GetTime ("/DEMOWebServices2.8/Service.asmx/GetTime?");

url::easy quest::HttpGet (APIHost+GetTime);

} //::re
