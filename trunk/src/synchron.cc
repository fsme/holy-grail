//@(#) Synchronize clock with server (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <sstream> 

#include <logostream.hpp> 

#include <synchron.h> 

using namespace std;
using namespace cxx;

namespace syn {

//
void chron::server_time (const std::string& res_)
{
	logs << info << "Init server time " << res_ << endl;
}

//
void chron::ize ( std::string& datetime )
{
	replace_if ( datetime.begin(), datetime.end()
				, bind2nd ( equal_to<char>(), 'T'), '\x20');

	string sdate, stime;
	struct tm cal;
	::bzero ( &cal, sizeof (struct tm) );

	std::stringstream sst (datetime);
	sst >> sdate >> stime;

	replace_if ( sdate.begin(), sdate.end()
				, bind2nd ( equal_to<char>(), '-'), '\x20');

	sst.str(""); sst.clear();
	sst << sdate << flush;
	sst >> cal.tm_year >> cal.tm_mon >> cal.tm_mday;
	cal.tm_year -= 1900;
	cal.tm_mon -= 1;

	string::size_type pos;
	if ( (pos = stime.find ('-')) != string::npos ) stime.erase (pos);

	replace_if ( stime.begin(), stime.end()
				, bind2nd ( equal_to<char>(), ':'), '\x20');

	sst.str(""); sst.clear();
	sst << stime << flush;
	sst >> cal.tm_hour >> cal.tm_min >> cal.tm_sec;

	cal.tm_gmtoff = 10800;

	std::mktime (&cal) ;
}

} //::syn

