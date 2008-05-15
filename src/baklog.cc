//@(#) Backup robotrade log
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <fstream>

#include <pstreams/pstream.h>

#include <logostream.hpp>
#include <iron.h>
#include <timeter.h>
#include <er.h>

using namespace std;
using namespace cxx;
using namespace env;


///\brief
inline
int32_t clock_to_sec (const std::string& tm_)
{
	std::string tm (tm_);
	std::string::size_type pos;
	for (pos = 0; pos <= tm.size(); ++pos)
		if (tm [pos] == ':') tm [pos] = ' ';

	int h,m,s;
	std::stringstream ssec (tm);
	ssec >> h >> m >> s;

	return h*3600+m*60+s;
}

///\brief Main, The
int
 main (
	  int argc
	, char* argv[]
) {

try{
	if (argc < 3)
		throw invalid_argument ("\
Usage: lar /path/to/robotrade.log /path/to/backup/dir");

	int32_t lacuna = 200;
	iron.getopt (argc, argv, "p:"); ///< -p #: Lacuna of time in second
	if ( iron.exists ("t"))
		lacuna = ::strtol ( iron ("t").c_str(), NULL, 10);

	ti::meter total ("Uptime (sec): ");

	ifstream file  ( argv [1]);
	if ( !file.is_open ())
		throw er::no ( std::string (argv[1])+ ": file not open");

	std::string str, mon, day, time0, mon1, day1, time1, mon2, day2, time2;
	int32_t prev = 0, next = 0;

	ofstream tmp ( (std::string (argv[2])+"/robotrade.tmp").c_str());
	if ( !tmp.is_open () )
		throw runtime_error ( std::string (argv[2])+"/robotrade.tmp"
							+": could not open");

	ofstream mycop ( (std::string (argv[2])+"/robotrade_system.log").c_str()
					, ios::app );

	if ( !mycop.is_open())
		throw runtime_error ( std::string (argv[2])+"/robotrade_system.log"
							+": could not open");

	while ( std::getline (file, str))
	{
		if ( str.find ("QUOTE") == string::npos)
		{
			mycop << str << endl;
			continue;
		}

		std::stringstream in (str);
		in >> mon >> day >> time0;
		if ( mon1.size() == 0) ///< Start date of log
		{
			mon1 = mon;
			day1 = day;
			time1 = time0;
		}

		next = clock_to_sec (time0);

		if (  prev && ( next - prev ) > lacuna )
		{
			std::string lar (day1+mon1+time1+"-"+day2+mon2+time2+"-robotrade");
			logs << "Backup to " << lar << endl;
			tmp.close ();
			if ( ::rename (\
						(std::string (argv[2])+"/robotrade.tmp").c_str()
					  , (std::string (argv[2])+"/"+lar).c_str()
					) < 0
			) throw runtime_error ( std::string ("Rename fail: ")
												+ ::strerror (errno));
			vector<string> args;
			args.push_back ("/usr/bin/tar");
			args.push_back ("-z");
			args.push_back ("-c");
			args.push_back ("-f");
			args.push_back ( (std::string (argv[2])+"/"+lar+".tar.gz").c_str());
			args.push_back ( (std::string (argv[2])+"/"+lar).c_str());

using namespace redi;
			ipstream tar ("tar", args);
			while ( !tar.rdbuf()->exited()) ::sleep (1);

			if (tar.rdbuf()->status() != 0)
				throw runtime_error ("tar error");

			::unlink ( (std::string (argv[2])+"/"+lar).c_str());

			tmp.open ( (std::string (argv[2])+"/robotrade.tmp").c_str());
			if ( !tmp.is_open ()) throw runtime_error ("tmp could not open");

			mon1.clear();
			day1.clear();
			time1.clear();
		}
		tmp <<  str << endl;

		mon2 = mon; day2 = day; time2 = time0;
		prev = next;
	}

} catch (const std::exception& e) {
		logs << error << e.what() << endl;
	}

}
