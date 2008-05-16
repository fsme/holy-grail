//@(#) Trading robot main, The function 
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <signal.h>

#include <logostream.hpp>
#include <iron.h>
#include <timeter.h>

#include <url_easy.hpp>
#include <memparser.hpp>

using namespace std;
using namespace cxx;
using namespace env;
using namespace mem;

//	std::string req (APIHost\
//		+ GetRatesServerAuth + iron ("efxnow_userid")
//		+ "&PWD=" + iron ("efxnow_password")
//		+ "&Brand=" + iron ("efxnow_brand")
//	);

///	std::string req (APIHost + Echo);

const string APIHost ("https://api.efxnow.com");
const string GetRatesServerAuth (\
"/DEMOWebServices2.8/Service.asmx/GetRatesServerAuth?UserID=");
string GetRatesDataSet("/DEMOWebServices2.8/Service.asmx/GetRatesDataSet?Key=");

const std::string Rates ("Rates");
const std::string Quote ("Quote");
const std::string Display ("Display");
const std::string UpdateTime ("UpdateTime");

map<string, string> mRates; ///< Current rates CCY => Rate

///\brief Sleep in milisecond
int milisleep (unsigned long milisec)
{
    struct timespec req={0};
    time_t sec=(int)(milisec/1000);
    milisec=milisec-(sec*1000);
    req.tv_sec=sec;
    req.tv_nsec=milisec*1000000L;
    while(nanosleep(&req,&req)==-1)
         continue;
    return 1;
}

///\brief Catch interrupts
void sig_action (int sig_ ///\param sig_ The signal
) {
	logs << notice << "Interrupt by signal " << sig_ << endl;
	exit (0);
}

///\brief Catch a signal
inline
void catch_signal ()
{
	::signal (SIGTERM, sig_action);
	::signal (SIGINT, sig_action);
}

///\brief
inline
void init_log ( const char* name_ ///\param name_ As Is argv[0]
) {
	if ( !iron.exists ("l") ) return;

	const char* _ident = name_;
	while ( *_ident++);
	while ( _ident != name_ && *(_ident-1) != '/')
		 _ident--;

	logs.open (_ident);
	logs.level (level_info);
}

int
 main (
	  int argc
	, char* argv[]
) {
	catch_signal ();

try{
	ti::meter total ("Uptime (sec): ");

	iron.getopt (argc, argv, "c:l"); ///< c - config file; l - write to syslog
	iron.configure ( iron ("c") );

	init_log ( argv[0]);
	std::string req (APIHost + GetRatesDataSet + iron ("efxnow_key"));

	url::easy GetRate (req);
	mem::parser http_response;

while (true)
{
	ti::meter timer ("Get URL (sec): ");
	if ( !GetRate.request()) {
		::sleep (10);
		continue;
	}

	try {
		http_response.parse ( GetRate.str().data(), GetRate.str().size() );
	}
     catch (const std::exception& e) {
			logs << error << e.what() << endl;
			::sleep (10);
			continue;
		} 

	xmlNode *cur = http_response.root_node->children->next->next->next->children->next->children->next;

	for (; cur; cur = cur->next)
	{
		if ( cur->type == XML_ELEMENT_NODE
		&&   Rates == (const char*)cur->name
		) {
			static std::string upquote, uprates, uptime;

			xmlNode* quot = cur->children;
			for (; quot; quot = quot->next)
			{
				if (quot->type == XML_ELEMENT_NODE)
				{
					if (Quote == (const char*)quot->name)
						 upquote = (const char*)quot->children->content;
					if (Display == (const char*)quot->name)
						 uprates = (const char*)quot->children->content;
					if (UpdateTime == (const char*)quot->name)
						 uptime = (const char*)quot->children->content;
				}
			}

			if (mRates [upquote] != uprates)
			{
				mRates [upquote] = uprates;
				logs << notice << "QUOTE "
					 << upquote << " " << uprates << " " << uptime << endl;
			}
			upquote.clear(); uprates.clear(); uptime.clear();
		}
	}
	::milisleep (500);

} //while forever

} catch (const std::exception& e) {
		logs << error << e.what() << endl;
	}

}
