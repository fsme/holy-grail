//@(#) Simple sum and simple average
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id: summ_unit.h 31 2008-05-14 18:39:24Z wingorodov $

#ifndef _SUMM_UNIT_H
#define _SUMM_UNIT_H 1

#include <math.h>

#include <logostream.hpp>

#include <average.hpp>

#include <order.h>

using namespace std;
using namespace cxx;
using namespace ave;
using namespace deal;

namespace summ {

///\class delta_unit
///\brief Sum and simple avarage of delta of the 

struct delta_unit
	: public basic_average<int32_t>
{
typedef basic_average<int32_t>	base_type;

///\brief Create
delta_unit (
	  const size_type size_ ///\param max_ Size for calculation
)
	: base_type (size_)
{}

///\brief Destroy
virtual ~delta_unit () {}

///\brief
virtual
void rehash (
		const fi::fo<int32_t>& fifo_ ///\param delts_
);

///\brief It is ready
///\return True if is it
virtual
	bool is_ready () const { return true; }
};


///\class direct_rates_fifo
///\brief Direct of last rates

class direct_rates
	: public ave::basic_direct<float>
	, public fi::fo<float>
{
typedef ave::basic_direct<float>	base_type;
typedef base_type::size_type 		size_type;
typedef fi::fo<float>				base_fifo;

public:

///\brief Create
direct_rates (
	  const size_t size_ ///\param max_ Size for calculation
)
	: base_type (size_)
	, base_fifo (size_)
{}

///\brief Destroy
virtual ~direct_rates () {}

///\brief
virtual
void push_back (
		const float& val_ ///\param val_
) {
	if ( base_fifo::is_full () ) base_fifo::pop_front();
	base_fifo::push_back (val_);

	if ( base_fifo::is_full () ) recalc ( *this );
}

///\brief Virtual recalc
virtual
void recalc (
		const fi::fo<float>& fifo_ ///\param fifo_ For calculation
) {
	bool up = true;
	bool down = true;

	base_fifo::size_type i;
	for (i = 1; i < base_type::size(); ++i )
	{
		if ( base_fifo::operator[] (i) >= base_fifo::operator[] (i-1) )
			down = false;

		if ( base_fifo::operator[] (i) <= base_fifo::operator[] (i-1) )
			up = false;
	}

	_dir ^= _dir;

	if ( !up && !down ) return;
	//else

//logs << "DIRECT: ";
	delta_rate dir_ ( base_fifo::front(), 0);
	for (i = 1; i <  base_type::size(); ++i )
	{
//logs << "" << base_fifo::operator[] (i) << " ? " << base_fifo::operator[] (i-1) << "|" ;
		dir_.rehash ( base_fifo::operator[] (i));
		_dir += dir_.delta;
	}
}

}; //.class direct_rates


///\class unit
///\brief Sum and simple sums with filling control

class unit
	: public basic_average<int32_t>
	, protected fi::fo<int32_t>
{
typedef fi::fo<int32_t>				base_fifo;
typedef basic_average<int32_t>		base_avrg;
typedef base_avrg::size_type		size_type;

public:

///\brief Create
unit (
	  const size_type size_ ///\param max_ Size for calculation
)
	: base_fifo (size_)
	, base_avrg (size_)
	, front_adder (size_)
	, _position (new deal::idle(0.0) )
	, _profit (0)
	, _real_profit (0)
	, _prophet(0)
	, sumax (3)
	, sumin (3)
	, _last (5)
	, cur_max (-100000)
	, cur_min (100000)
	, direct_zero_point (3)
	, minRates (3)
	, maxRates (3)
	, min_rate (100000.0)
	, max_rate (0.0)
	, _conformity (07777)
	, tic (0)
	, cur_sum_tic (0)
	, cur_rate_tic (0)
{}

///\brief Destroy
virtual ~unit () {}

///\brief Recalculate at new data
void recalc (
	  const float bid_ ///\param bid_ Last Bid
	, const float ask_ ///\param ask_ Last Ask
	, const fi::fo<int32_t>& fifo_ ///\param fifo_ Delta FIFO primary
);

///\brief
virtual
void push_back ( const int32_t& sum_ ///\param sum_
);

///\brief
virtual
void rehash (
		const fi::fo<int32_t>& fifo_ ///\param fifo_
);

///\brief Get filling in percent
///\return Filling in percent (0-100) 
int32_t fill () const;

///\brief Conformity rate with sums
///\return Conformity in pro mil
int32_t conf () const { return _conformity; }

///\brief It is ready
///\return True if is it
virtual
	bool is_ready () const { return sumax.is_full() && sumin.is_full(); }

///\brief Get size
///\return Size
	size_type size () const { return base_avrg::size(); }

///\brief Result of deals
///\return The profit in pipes
int32_t
	profit () const { return  _profit; }

///\brief Result of deals LIVE
///\return The profit in pips
int32_t
	real_profit () const { return  _real_profit; }

///\brief Looks nice, try it for real live
///\return True if bingo
bool
	is_cool () const
{
	return true; 
}

///\brief Amount of lucrative deals
///\return Number from last zero
int32_t
	prophet () const { return  _prophet; }

///\brief Direct by Zero points
///\return 
int32_t
	direct () const { return  direct_zero_point.direct() ; }

///\brief U turn the direct of this trend at 5 last point
///\return True if trend is u turn), else false 
bool
	u_turn () const
{
	if (_last.size() <  5 || _last.back() == 0) return false;

	if ( 0 < _last.back () )
		return ( (_last[0] > _last[2]) && (_last[2] > _last[4] ) );

	if ( 0 > _last.back () )
		return ( (_last[0] < _last[2]) && (_last[2] < _last[4] ) );
}

///\brief 
virtual
void print (
	  const std::string& pre_ = "" ///\param pre_ Prefix
	, const std::string& col_ = " "///\param pre_ Divider (colon)
) const {
	minRates.print (" min_rates=", col_);
	maxRates.print (" MAX_RATES=", col_);
	sumax.print ("sumax=", col_);
	sumin.print ("sumax=", col_);
}

///\brief Forecast of profit
///\return Forecast of profit in pips

int32_t forecast () const;

private:

///\brief Clean up position pointer
float delete_position ()
{
	if (_position <= 0) return 0.0;

	float bid_ = _position->bid ();
	delete _position;
	_position = 0;

	return bid_;
}

///\brief Make deal, check out position
void make_deal ();

	delta_unit	front_adder; ///< Front-end summ unit
	float		last_bid;
	float		last_ask;

	int32_t cur_max;
	fifo_avrg<int32_t> sumax;

	int32_t cur_min;
	fifo_avrg<int32_t> sumin;

	float min_rate;
	fifo_avrg<float> minRates;

	float max_rate;
	fifo_avrg<float> maxRates;

	direct_rates direct_zero_point;

	int32_t	tic;
	int32_t	cur_sum_tic;
	int32_t	cur_rate_tic;

	int32_t _conformity;

	float	_forecast;
	int32_t	_profit;
	int32_t	_real_profit;
	int32_t	_prophet;

	fi::fo<int32_t> _last;

	deal::order* _position;
};

///\brief Compare units by profit
///\return True if profit is more then
inline
bool more_by_profit (
	  const unit& unit_1 ///\param unit_1 
	, const unit& unit_2 ///\param unit_2 
) {
	return unit_1.profit () > unit_2.profit ();
}

///\brief Compare units by prophet
///\return True if prophet is more then
inline
bool more_by_prophet (
	  const unit& unit_1 ///\param unit_1 
	, const unit& unit_2 ///\param unit_2 
) {
	return unit_1.prophet () > unit_2.prophet ();
}


} //::summ

#endif //!_SUMM_UNIT_HPP
