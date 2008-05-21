//@(#) FIFO of statistics
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _STAT_FIFO_H
#define _STAT_FIFO_H 1

#include <logostream.hpp>

#include <fifo.hpp>
#include <delta_rate.h>

using namespace std;
using namespace cxx;
using namespace fx;

namespace stat {

///\class fifo
///\brief FIFO of statistics: Bid, Ask and Delta

class fifo
{
typedef fi::fo<int32_t>::size_type	size_type;

public:

///\brief Create
fifo () {}

///\brief Destroy
~fifo () {}

///\brief Set new size of statistic
void resize ( const int32_t size_ ///\param size_ New size
) {
	AskFIFO.max_size (size_);
	BidFIFO.max_size (size_);
	DltFIFO.max_size (size_);
}

///\brief Push new data
void
	push (
		  const float bid_ ///\param bid_ New bid
		, const float ask_ ///\param ask_ New ask
) {
	static delta_rate _Bid;
	static delta_rate _Ask;

	_Bid.rehash (bid_);
	_Ask.rehash (ask_);

	BidFIFO.push_back ( _Bid.rate );
	AskFIFO.push_back ( _Ask.rate );
	DltFIFO.push_back ( _Bid.delta );
}

///\brief Is ready
///\return True if cleared for action
bool
	is_ready () const { return DltFIFO.is_full(); }

///\brief Get size
///\return Current FIFO size
size_type
	size () const { return DltFIFO.size(); }

///\brief Get BID
///\return The BidFIFO
const fi::fo<float>& bids () const { return BidFIFO; }

///\brief Get ASK 
///\return The AskFIFO
const fi::fo<float>& asks () const { return AskFIFO; }

///\brief Get Delta
///\return The DltFIFO
const fi::fo<int32_t>& delta () const { return DltFIFO; }

///\brief Unit test
///\return True if OK
bool unitst ()
{
	return true;
}

///\brief Clear statistic (save 2 last)
void clear ()
{
	DltFIFO.soft_clear ();
	BidFIFO.soft_clear ();
	AskFIFO.soft_clear ();
}

private:

static fi::fo<float>	BidFIFO;	///< FIFO of Bids
static fi::fo<float>	AskFIFO;	///< FIFO of Asks
static fi::fo<int32_t>	DltFIFO;	///< FIFO of delta

}; //.class fifo


///\class inst
///\brief Instance of FIFOs
struct ra {
	static fifo& tes () {
		static fifo _rates;
		return _rates;
	}
};

///\brief Get last Delta
///\return The
inline
int32_t last_delta () { return ra::tes().delta().back(); }

///\brief Get last Bid
///\return The
inline
float last_bid () { return ra::tes().bids().back(); }

///\brief Get previous Bid
///\return The
inline
float prev_bid () { return ra::tes().bids()[ ra::tes().size()-2 ]; }

///\brief Get last Ask
///\return The
inline
float last_ask () { return ra::tes().asks().back(); }

///\brief Get previous Ask
///\return The
inline
float prev_ask () { return ra::tes().asks()[ ra::tes().size()-2 ]; }


} //::stat

#endif //!_STAT_FIFO_H
