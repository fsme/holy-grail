//@(#) Make deal
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id: trade.cc 31 2008-05-14 18:39:24Z wingorodov $

#include <signal.h>
#include <math.h>

#include <bitset>

#include <logostream.hpp>
#include <iron.h>
#include <timeter.h>
#include <er.h>

#include <ratestream.hpp>
#include <rate_stats.h>
#include <factor.h>

using namespace std;
using namespace cxx;
using namespace env;
using namespace fx;

bool shut_down = false;

///\brief Catch interrupts
void sig_action (int sig_ ///\param sig_ The signal
) {
	logs << notice << "Interrupt by signal " << sig_ << endl;
	exit (0);
}

///\brief Clock to second
inline
 long clock_to_sec ( const std::string& tm_ ///\param tm_ Date-time string
) {
	std::string tm (tm_);
	std::string::size_type pos;
	for (pos = 0; pos <= tm.size(); ++pos)
		if (tm [pos] == ':') tm [pos] = ' ';

	int h,m,s;
	std::stringstream ssec (tm);
	ssec >> h >> m >> s;

	return h*3600+m*60+s;
}

///\brief Clock to second
///\return 
inline
 void rate_to_float (
	  std::string& rate_ ///\param
	, float& float_bid	///\param
	, float& float_ask	///\param
) {
	std::string::iterator i = rate_.begin();
	for (; i != rate_.end(); ++i)
		if ( *i == '.' || *i == '/' ) *i = ' ';

	std::stringstream sstr (rate_);
	int int_base, int_bid, int_ask;

	sstr >> int_base >> int_bid >> int_ask;

	float_bid = (float)int_base + ( (float)int_bid / fx::multi::factor() );

	switch ( fx::multi::factor())
	{
	 case 100:
		float_ask = (float)int_base + ( (float)int_ask / fx::multi::factor() );
		if ( float_ask < float_bid ) float_ask += 1;
		break;

	 case 10000:
		int ost = int_bid % 100;		
		int_bid = int_bid / 100;		
		if (int_ask < ost)
		{
			int_bid = int_bid * 100 + ost + (100-ost);
			int_bid = int_bid / 100;		
		}
		int_ask = int_bid * 100 + int_ask;
		float_ask = (float)int_base + ( (float)int_ask / fx::multi::factor() );
		break;
	}
}

stat::rates StatRates (377);

///\brief Main, The
int
 main (
	  int argc
	, char* argv[]
) {
	::signal (SIGTERM, sig_action);
	::signal (SIGINT, sig_action);

try {
	ti::meter total ("Uptime (sec): ");
	if (argc < 4)
 		throw invalid_argument ("Usage: trade -y CCY {/path/to/quotes.log|-}");

	env::iron.getopt ( argc, argv, "y:r"); ///< r - real deal
	
	fx::multi::factor ( iron ["y"]);
	if ( fx::multi::factor () == 0 )
		throw invalid_argument ( iron ["y"]+ " CCY unknown");
	
	std::string ifname (argv[3]); ///< Input from
	if ( ifname [0] == '-') ifname = "/dev/stdin";

	ifrate quoteslogfile (ifname);
	if ( !quoteslogfile .is_open ())
		throw er::no (ifname+": open error ");

	std::string quote, rate, timest;
	float bid, ask;

	while ( std::getline (quoteslogfile, quote))
	{
		if (shut_down) break;

		std::stringstream input (quote);
		while (input >> quote)
		{
			if (quote != "QUOTE") continue;

			input >> quote >> rate >> timest;

			if (quote != iron ("y")) continue;

			rate_to_float (rate, bid, ask);

			StatRates.push (bid, ask, clock_to_sec (timest));
		}
	}

} catch (const std::exception& e) {
		logs << error << e.what() << endl;
	}

}
