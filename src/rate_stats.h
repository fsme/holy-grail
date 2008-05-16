//@(#) Rates statistics as a finite state machine
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _STAT_RATES_H
#define _STAT_RATES_H 1

#include <vector>

#include <logostream.hpp>

#include <delta_rate.h>
#include <factor.h>

#include <summ_unit.h>

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
rates ( const size_type& size_///\param max_ Size of data set of the rates
)
	: _profit (0)
{
	AskFIFO.max_size (size_);
	BidFIFO.max_size (size_);
	DeltaFIFO.max_size (size_);

	for ( size_type i = size_; i > 54; --i )
		ds_units.push_back ( summ::unit (i) );
}

///\brief Destroy
~rates () {}

///\brief Push new rate
void
	push (
		  const float bid_ ///\param bid_ New bid
		, const float ask_ ///\param ask_ New ask
) {
	static delta_rate _bid;
	static delta_rate _ask;

	_bid.rehash (bid_);
	_ask.rehash (ask_);

	BidFIFO.push_back ( _bid.rate );

	DeltaFIFO.push_back ( _bid.delta );

	AskFIFO.push_back ( _ask.rate );

	transit ();
}

///\brief Transition to the new action
void
	transit ()
{
logs << "" << BidFIFO [ BidFIFO.size()-1 ] << " "; 
//	 << "" << AskFIFO [ AskFIFO.size()-1 ] << "  ";

	_profit = 0;
	vectorDeltaSummFIFO::iterator adder = ds_units.begin();
	bool srt = false;
	int num = 0;

	for (; adder != ds_units.end (); ++adder )
	{
		adder->recalc ( BidFIFO.back (), AskFIFO.back (), DeltaFIFO );

		_profit += adder->real_profit();

	 	if (  adder->is_ready () && adder->sum () == 0) srt = true;
		if ( !adder->is_ready () || ++num > 6 ) continue;

		logs << "#" << adder->size()
//			 << "|" << adder->sum()
			 << "($" << adder->profit() << ")";
//			 << "<" << adder->prophet() << "> ";

//		if ( adder->u_turn())
//			logs << "<" << adder->direct () << ">"
//			 	 << "{$" << adder->forecast() << "}";

	logs << " ";

	}//for

//logs << " __$" <<  profit() << "__ " << endl;
	logs << "" << endl;

	if (srt)
	std::stable_sort ( ds_units.begin(), ds_units.end(), summ::more_by_prophet);

}

///\brief Get real profit
///\return Real live profit
int32_t
	profit () const { return _profit; }

///\brief Is ready
///\return True if cleared for action
//bool
//	is_ready () const { return false; }

///\brief Get size
///\return Current FIFO size
size_type
	size () const { return DeltaFIFO.size(); }

///\brief Show queue
void print (
	  const std::string& pre_ = "" ///\param pre_ Prefix before show
	, const std::string& col_ = " " ///\param col_ Colon for div
) const {
	logs << notice  << "STAT RATES PRINT" << endl;
}

///\brief Unit test
///\return True if OK
bool unitst ()
{
	return true;
}

private:
static fi::fo<float>	BidFIFO;	///< FIFO of Bids
static fi::fo<float>	AskFIFO;	///< FIFO of Asks
static fi::fo<int32_t>	DeltaFIFO;	///< FIFO of delta

	vectorDeltaSummFIFO		ds_units; ///< Delta summs statistic units

	int32_t		_profit;

}; //.class rates

} //::stat

#endif //!_STAT_RATES_H
