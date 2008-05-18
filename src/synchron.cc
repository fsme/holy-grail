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
void chron::server_time (
		  const std::string& res_
		, time_t timer_
) {
cout << " GetTime=" << res_;

	std::string date_ (res_);
	replace_if ( date_.begin(), date_.end()
			, bind2nd ( equal_to<char>(), '/'), '\x20');

	replace_if ( date_.begin(), date_.end()
			, bind2nd ( equal_to<char>(), ':'), '\x20');

	std::stringstream sst; sst << date_ << endl;
	std::string yr, mon, day, hr, min, sec, am;

	sst >> mon >> day >> yr >> hr >> min >> sec >> am;
	if (am == "PM")
	{
		int hour;
		sst << hr << endl;
		sst >> hour;
		hour += 12;
		sst << hour << endl;
		sst >> hr;
	}

	date_ = yr+"-"+mon+"-"+day+"T"+hr+":"+min+":"+sec+"-04:00";
	
	sync ( timer_ - date_to_unixtime (date_) + (time_t)14400);
}

//
time_t chron::date_to_unixtime ( std::string& date_ )
{
	replace_if ( date_.begin(), date_.end()
				, bind2nd ( equal_to<char>(), 'T'), '\x20');

	string sdate, stime;
	struct tm cal;
	::bzero ( &cal, sizeof (struct tm) );

	std::stringstream sst (date_);
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

	return std::mktime (&cal) ;
}

//
void chron::quote_time ( std::string& date_ )
{
	_quote_time = date_to_unixtime (date_);
	static time_t prev_time = _quote_time;

	_lacuna = (int32_t)(_quote_time - prev_time);
	prev_time = _quote_time;
}

} //::syn

