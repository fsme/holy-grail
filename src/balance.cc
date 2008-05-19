//@(#) Get margin and position blotter
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <signal.h>

#include <logostream.hpp>
#include <iron.h>
#include <timeter.h>
#include <er.h>

#include <factor.h>
#include <request.h>

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

///\brief Extract content
inline
std::string&
	get_node_content ( std::string& line_///\param line_ Blotter's string
) {
	string::size_type pos1, pos2;

	if ( (pos1 = line_.find ("&gt;")) == string::npos) return line_;
	if ( (pos2= line_.find ("&lt;/")) == string::npos) return line_;

	pos2 = pos2 - pos1 - 4;
	line_.assign (line_, pos1+4, pos2);

	return line_;
}

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
	logs.level (level_info);

	env::iron.getopt (argc, argv,"c:");

	if ( iron.exists ("c"))
		iron.configure ( iron ("c"));
	else
		iron.configure ("/home/ass/etc/passwd");

	std::stringstream blotter ( re::que::st().get_margin());

	std::string line;
	string::size_type pos, pos2;

	while ( std::getline ( blotter, line ))
	{
		if ( line.find ("MarginBalance") != string::npos ) 
			logs << notice
				 << "MarginBalance:\t$" << get_node_content (line) << endl;

		if ( line.find ("USDRealizedProfit") != string::npos ) 
			logs << notice
				 << "Realized:\t$ " << get_node_content (line) << endl;

		if ( line.find ("UnrealizedProfit") != string::npos ) 
			logs << notice
				 << "Unrealized:\t$ " << get_node_content (line) << endl;
	}

	blotter.str("");
	blotter.clear();
	blotter << re::que::st().get_position();

	while ( std::getline ( blotter, line ))
	{
		if ( line.find ("PositionBlotter") != string::npos )
		{
			string _pair, _amount, _profit;
			while ( std::getline ( blotter, line ))
			{
				if ( line.find ("PositionBlotter") != string::npos ) break;
				if ( line.find ("Symbol") != string::npos )
				{
					_pair = get_node_content (line);
					continue;
				}
				if ( line.find ("ContractPosition") != string::npos )
				{
					_amount = get_node_content (line);
					continue;
				}
				if ( line.find ("Unrealized") != string::npos )
				{
					_profit = get_node_content (line);
					continue;
				}
			}//.while

		if ( _amount != "0" )
			 logs << _pair<< "\t" << _amount << " <" << _profit << ">" << endl;
		}
	}

} catch (const std::exception& e) {
		logs << error << e.what() << endl;
	}

}
