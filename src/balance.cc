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
try {
	ti::meter total ("Uptime (sec): ");
	logs.level (level_info);

	iron.getopt (argc, argv, "c:");

	if ( !iron.exists ("c"))
	{
		if ( argv[0])
		{
			std::string param ( argv[0]);
			if ( param.find_first_of ("123456") != string::npos )
				 iron ["c"] = string ("AW") + param + "_forex_com";
			else
				 iron ["c"] = "demo_forex_com";
		} else {
				 iron ["c"] = "demo_forex_com";
		}
	}

	iron.getenv ("HOME");
	iron.configure ( iron ("HOME")+"/etc/"+iron("c") );

	clo::ck();
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
