//@(#) Process a rates statistics
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _STAT_RATES_H
#define _STAT_RATES_H 1

#include <vector>

#include <logostream.hpp>

#include <stat_fifo.h>
#include <summ_unit.h>
#include <synchron.h>

using namespace std;
using namespace cxx;
using namespace ave;
using namespace fx;

namespace stat {

///\class rates
///\brief Rates statistic FSM

class rates
{
typedef fi::fo<int32_t>::size_type	size_type;
typedef	vector<summ::unit>	vectorDeltaSummFIFO;

public:

///\brief Create
rates ()
	: _profit (0)
{
	//for ( size_type i = size(); i > 33; --i )
		ds_units.push_back ( summ::unit (69));
}

///\brief Destroy
~rates () {}

///\brief Transition to the new action
void transit ()
{
	if ( is_ready()
	&&  ( clo::ck().lacuna() > 300 && (last_delta() > 2 || last_delta() < -2) )
	||  ( last_delta() > 15 || last_delta() < -15 )
	) {
		if (logs << info)
			logs << "Lacuna="<< clo::ck().lacuna()
				 << "; Delta=" << last_delta()
				 << "; clear statistic" << endl;
		clear();
	}	

	if (logs << info)
		logs << "" << last_bid () << "/" << "" << last_ask () << " ";

	int32_t prof_ = profit();
	_profit = 0;

	vectorDeltaSummFIFO::iterator adder = ds_units.begin();
	bool srt = false;
	int num = 0;

	for (; adder != ds_units.end (); ++adder )
	{
		adder->recalc (num);

		_profit += adder->real_profit();

	 	if (  adder->is_ready () && adder->sum () == 0) srt = true;
		if ( !adder->is_ready () || ++num > 6 ) continue;

	if (logs << info)
		logs << " #" << adder->size()
			 << " {" << adder->sum() << "}" 
			 << " $" << adder->profit();

	if ( adder->forecast() > 0 )
		logs << " <" << adder->forecast() << "|" << adder->direct() << "> ";

	}//for

///	if (prof_ != profit()
	if (logs << info)
	 	logs << " [" << profit() << "]" << endl;


	if (srt)
	std::stable_sort (\
		 ds_units.begin(), ds_units.end(), summ::more_by_profit );
}

///\brief Get real profit
///\return Real live profit
int32_t
	profit () const { return _profit; }

///\brief Is ready
///\return True if cleared for action
bool
	is_ready () const { return ra::tes().is_ready(); }

///\brief Get size
///\return Current FIFO size
size_type
		size () const { return ra::tes().size(); }

///\brief Unit test
///\return True if OK
bool unitst ()
{
	return true;
}

private:

vectorDeltaSummFIFO		ds_units; ///< Delta summs statistic units
int32_t		_profit;

///\brief Clear all statistic
void clear ()
{
	vectorDeltaSummFIFO::iterator adder = ds_units.begin();
	for (; adder != ds_units.end (); ++adder )
		adder->clear();

	ra::tes().clear ();
}

}; //.class rates


} //::stat


namespace deal {

///\class eng
///\brief Auto processing singleton

struct en
{
	static stat::rates& gine ()
	{
		static stat::rates engine;
		return engine;
	}

}; //.class eng

}//::deal

#endif //!_STAT_RATES_H
