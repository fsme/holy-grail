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
rates ( const size_type& size_///\param max_ Size of data set of the rates
)
	: _profit (0)
{
	AskFIFO.max_size (size_);
	BidFIFO.max_size (size_);
	DltFIFO.max_size (size_);

	for ( size_type i = size_; i > 33; --i )
		ds_units.push_back ( summ::unit (i));
}

///\brief Destroy
~rates () {}

///\brief Push new rate
void
	push (
		  const float bid_ ///\param bid_ New bid
		, const float ask_ ///\param ask_ New ask
) {
	static delta_rate _Bid;
	static delta_rate _Ask;

	_Bid.rehash (bid_);
	_Ask.rehash (ask_);

	if ( clo::ck().lacuna() > 500)
	{
		if (logs << info)
			logs << "Lacuna="<< clo::ck().lacuna() << "; clear stats"
				 << endl;
		clear();
	}	

	BidFIFO.push_back ( _Bid.rate );
	AskFIFO.push_back ( _Ask.rate );
	DltFIFO.push_back ( _Bid.delta );

	transit ();
}

///\brief Transition to the new action
void
	transit ()
{
if (logs << info)
	logs << "" << BidFIFO [ BidFIFO.size()-1 ] << "/"
		 << "" << AskFIFO [ AskFIFO.size()-1 ] << " ";

	int32_t prof_ = profit();
	_profit = 0;
	vectorDeltaSummFIFO::iterator adder = ds_units.begin();
	bool srt = false;
	int num = 0;

	for (; adder != ds_units.end (); ++adder )
	{
		adder->recalc ( BidFIFO.back (), AskFIFO.back (), DltFIFO, num );

		_profit += adder->real_profit();

	 	if (  adder->is_ready () && adder->sum () == 0) srt = true;
		if ( !adder->is_ready () || ++num > 8 ) continue;

	if (logs << info)
		logs << "#" << adder->size()
	//		 << "|" << adder->sum()
			 << "($" << adder->profit() << ") ";

	}//for

	if (logs << info) logs << " " << endl;
	

	if (prof_ != profit()
	&& (logs << info)
	) 	logs << "Result=" << profit() << endl;


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
//bool
//	is_ready () const { return false; }

///\brief Get size
///\return Current FIFO size
size_type
	size () const { return DltFIFO.size(); }

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
static fi::fo<int32_t>	DltFIFO;	///< FIFO of delta

	vectorDeltaSummFIFO		ds_units; ///< Delta summs statistic units

	int32_t		_profit;


///\brief Clear all statistic
void clear ()
{
	BidFIFO.clear ();
	DltFIFO.clear ();
	AskFIFO.clear ();

	vectorDeltaSummFIFO::iterator adder = ds_units.begin();
	for (; adder != ds_units.end (); ++adder )
		adder->clear();
}

}; //.class rates

} //::stat

#endif //!_STAT_RATES_H
