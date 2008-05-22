//@(#) Adder unit and super statistic (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#include <summ_unit.h>
#include <delta_rate.h>
#include <stat_fifo.h>

#include <sstream>

using namespace std;
using namespace cxx;
using namespace ave;
using namespace summ;
using namespace stat;

namespace summ {

///\class delta_unit

//
void delta_unit::rehash ( const fi::fo<int32_t>& fifo_ ///\param fifo_  ...
) {
	if ( base_type::size() > fifo_.size()) return;

	_sum ^= _sum;
	size_type i = fifo_.size() - base_type::size();
	for ( ; i < fifo_.size(); ++i ) {
		_sum += fifo_ [i];
	}
}


//\class unit

//
void unit::recalc (
	const int32_t rating_
) {
	_rating = rating_;

	front_adder.rehash ( ra::tes().delta() );
	push_back ( front_adder.sum ());
}


//
void unit::push_back (
		const int32_t& sum_ 
) {
	if ( base_fifo::is_full () ) base_fifo::pop_front();
	base_fifo::push_back (sum_);
	
	rehash (*this);
	make_deal ();
}


//
void unit::rehash (
		const fi::fo<int32_t>& fifo_ 
) {
	if ( base_avrg::size() > fifo_.size()) return;

	int32_t _save = sum ();
	_sum ^= _sum;
	size_type i = fifo_.size() - base_avrg::size();

	for ( ; i< fifo_.size(); ++i) _sum += fifo_ [i]; //!REFACTORING

	if ( (sum() > 0 && _save < 0)
	||   (sum() < 0 && _save > 0)
	)
	 _sum ^= _sum; ///< Fixup zero point

	_last.push_back (_sum);

	if ( 0 != sum ()) ///< Extreme points
	{
		if ( sum() > 0 && (sum () > cur_max )) cur_max = sum ();
		if ( sum() < 0 && (sum () < cur_min )) cur_min = sum ();

		if ( sum() > 0 && (max_rate < last_bid()) ) max_rate = last_bid();
		if ( sum() < 0 && (min_rate > last_bid()) ) min_rate = last_bid();

	} else {

		direct_zero_point.push_back ( last_bid());

		if ( _save > 0) {
			sumax.push_back (cur_max);
			cur_max = -100000;

			maxRates.push_back (max_rate);
			max_rate = -10000.0;
		} 
		if ( _save < 0) {
			sumin.push_back (cur_min);
			cur_min = 100000;

			minRates.push_back (min_rate);
			min_rate = 10000.0;
		}
	}
}


//
inline
void unit::make_deal ()
{
	if ( ( last_delta() > 10 || last_delta() < -10)
	&&   clo::ck().lacuna() < 15 ///< Flight! Cash in ON
	) {
		if ( _position->is_open ())
		{
			if (_position->is_long() && last_delta() > 0)
				_position->trail_target (12, 100.0);
		}
	}

	///< Signal disappeared
	if (!_position->is_closed ()
	&&   _position->is_action()
	&&  !_position->is_open()
	&&	!u_turn()
	) {
		_position = new deal::idle ();
	}

	_position->checkout ();

	if ( _position->is_closed () && sum() == 0 )
	{
		if (_position->is_real() )
			_real_profit += _position->profit();

		if (_position->profit() != 0)
			if (_position->profit() > 0) ++_prophet; else --_prophet;
		_profit += _position->profit();

		_proph.push (_position->profit());

		if (logs << info)
			logs << "Size=" << size()
				 << " History=";

		print_history ();

		if (logs << info)
			logs << " Profit=" << _position->profit()
				 << " Real=" << boolalpha << _position->is_real()
				 << endl;

		_position = new deal::idle ();
	}

	if ( !_position->is_closed()
	&&   !_position->is_action()
	&&   u_turn()
	&&   forecast() > 2
	) {
		if ( _last.back() > 0 && direct() < -11 )
			_position = new deal::sell ( 3, is_cool ());

		if ( _last.back() < 0 && direct() > 11 )
			_position = new deal::buy ( 3, is_cool ());
	}
}

//
int32_t unit::fill () const
{
	if ( sum() > 0 && sumax.avrg() != 0)
		return ( ::lroundf ( sum()*100 / sumax.avrg() ) );

	if ( sum() < 0 && sumin.avrg() != 0)
		return ( ::lroundf ( sum()*100 / sumin.avrg() ) );

	return 0;
}

//
int32_t unit::forecast () const
{
	if ( !is_ready() || !u_turn()) return 0;

	fx::delta_rate profit_ ( last_bid(), 0);
	profit_.rehash (\
		 ( 0 > _last.back() ) ? maxRates.avrg () : minRates.avrg ()
	);

	return (0 < _last.back ()) ? -profit_.delta : profit_.delta;
}

} //::summ

