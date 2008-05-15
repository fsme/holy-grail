//@(#) Order for deal (header)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _DEAL_ORDER_H
#define _DEAL_ORDER_H 1

#include <stdexcept>
#include <cmath>

#include <boost/thread/mutex.hpp>

#include <logostream.hpp>

#include <factor.h>
#include <delta_rate.h>
#include <request.h>

using namespace std;
using namespace cxx;
using namespace fx;

namespace deal {

///\class order
///\brief Virtual order

class order
{
public:

///\brief Create
order (
	  const float bid_///\param trail_ Current bid
	, const int32_t trail_///\param trail_ Trailing start
	, const bool try_for_real_///\param try_for_real_ Maybe real order, try it
)
	: _bid (bid_)
	, _open_rate (0.0)
	, _target (0.0)
	, _trail (trail_)
	, _profit (0)
	, _is_open (false)
	, _is_closed (false)
	, _that_real (try_for_real_)
	, _this_id (::rand () + 1)
{}

///\brief Destroy
virtual ~order ()
{
	if ( is_real() ) lock_for_real (id(), false);
}

///\brief Get ID
///\return Identifier of this
int32_t id () const { return _this_id;  }

///\brief Get current Bid
///\return Current Bid for deal
float bid () const { return _bid;  }

///\brief Get current Ask
///\return Current Ask for deal
float ask () const { return _ask;  }

///\brief Get current profit
///\return Current profit
int32_t profit () const { return _profit;  }

///\brief Check for action
///\return True if trailing stop more then zero
bool is_action () const { return _trail >= 0; }

///\brief Check for open
///\return True if open
bool is_open () const { return _is_open; }

///\brief Check for closed
///\return True if closed
bool is_closed () const { return _is_closed; }

///\brief Is real live
///\return True if real of deal
bool is_real () { return _that_real; }

///\brief Stop now
void stop () { _bid = 0.0; _ask = 0.0; _trail = -1; }

///\brief Set trailing for sell
virtual
void trail (
	  const int32_t trail_ ///\param New trailing 
	, float new_rate ///\param new_rate Rate for calculate
) = 0;

///\brief Check out current rate
virtual
void check (
	  float bid_ ///\param New Bid
	, float ask_ ///\param New Ask
) = 0;

protected:
	float	_bid;		///< Bid for deal
	float	_ask;		///< Ask for deal

	float	_open_rate; ///< Rate save open position
	float	_trailing_stop; ///< Rate for trailing
	float	_target;	///< Rate for trailing

	int32_t	_trail;		///< Trailing in pips
	int32_t _profit;	///< At pipies
	int32_t	_this_id;	///< Identifier of order, random

	bool	_is_open;	///< In the deal
	bool	_is_closed;	///< Outside
	bool	_that_real;	///< Real live


///\brief Try lock for real deal
///\return True if locked; False if unlocked
bool lock_for_real (
	  const	int32_t id_ ///\param id_ Identifier (random) of order
	, const bool lock_ ///\param lock_ Lock or unlock
);

///\brief Real request for BUY
///\return False if fail
bool
	real_buy ( float& ask_ ///\param ask_ Real ASK
) {
/*****
	DealRequest.setURL (APIDemoHost+Echo+"BUY");
	if ( !DealRequest.request()) return false;

	logs << "" << endl << DealRequest.str() << endl;
*******/
	return true;
}

///\brief Real request for SELL
///\return False if fail
bool
	real_sell ( float& bid_ ///\param bid_ Real BID
) {
/*****
	DealRequest.setURL (APIDemoHost+Echo+"SELL");
	if ( !DealRequest.request()) return false;

	logs << "" << endl << DealRequest.str() << endl;
*****/
	return true;
}

}; //.order


///\class buy
///\brief Order for BUY

struct buy
	: public order
{
	using order::_trail;
	using order::_bid;
	using order::_ask;
public:

///\brief Create
buy (
	  const float rate_ ///\param trail_ Current rate
	, const int32_t trail_= 3 ///\param trail_ Trailing start
	, const bool try_for_real_ = false ///\param try_for_real_ Try it
)
	: order (rate_, trail_, try_for_real_)
{
	_ask = bid() + 100.0 / multi::factor();
	_trailing_stop = ask() + (float) _trail / multi::factor();

logs << " CREATE ORDER BUY ID=" << id() << " START=" << _trailing_stop << " ";
}

///\brief Destroy
virtual ~buy () {
//logs << " DELETE ORDER BUY ";
}

///\brief Set new trailing for BUY
virtual
void trail (
	  const int32_t trail_ ///\param New trailing 
	, float new_rate = 0.0 ///\param new_rate Rate for calculate
) {
	if ( !is_action() && !is_open() ) return;
	_trail = trail_;
	float trailstop = (is_action ()) ? -_trail : _trail;

	if ( 0.0 == new_rate) new_rate = ( is_open() ) ? bid() : ask();
	float new_stop = new_rate + trailstop  / multi::factor();

	if ( is_open ())
	{
		if ( new_stop > _trailing_stop ) { _trailing_stop = new_stop;

//logs << "TRAIL for OPEN STOP=" << _trailing_stop << " ";

		}
	}
	else // is_action ()
		if ( new_stop < _trailing_stop) { _trailing_stop = new_stop;

//logs << "TRAIL ACTION STOP=" << _trailing_stop << " ";
			}
}

///\brief Check out current rate for BUY
virtual
void check (
	  float bid_ ///\param New Bid
	, float ask_ ///\param New Ask
) {
	if ( is_closed () || !is_action() ) return;

	if ( is_open ())
	{
/*******
		if (bid_ > _target)
		{
			_target = bid_ + 10.0 / multi::factor();
			trail (1, bid_);
		}
********/
		if ( bid_ < _trailing_stop  ///< CLOSE LONG
		||  (bid_ > _target)
		)  {

			if ( is_real())
			{
				if ( !real_sell (bid_))
					throw runtime_error ("Real SELL was FAIL");

				lock_for_real ( id(), false);
			}

			delta_rate pf (_open_rate, 0);
			pf.rehash (bid_);
			_profit = pf.delta;

logs << "CLOSE LONG BUY at $" << bid_ << "; PROFIT=" << _profit << "; ";

			_is_closed = true;
			_is_open = false;

			return;
		}

		if ( /* new */ bid_ > _bid ) ///< POP UP TRAILING STOP
		{
			float new_stop = bid_ - (float) _trail / multi::factor();
			if ( new_stop > _trailing_stop) { _trailing_stop = new_stop; }

/***
			///< FIX $1
			new_stop = _open_rate + 2.0 / multi::factor();
			if (bid_ > new_stop
			&& _trailing_stop < new_stop)  _trailing_stop = new_stop; 
****/

//logs << "STOP for BUY $" << _trailing_stop << " NEW STOP=" << new_stop << " ";

		}

		_bid = /* new */ bid_; 
		_ask = /* new */ ask_ ; 
		
		return;
	}

	//--IS_ACTION

	if ( /* new */ ask_ < _ask ) ///< PUSH DOWN TRAILING START
	{
		float new_start = ask_ + (float) _trail / multi::factor();
		if ( new_start < _trailing_stop ) { _trailing_stop = new_start; }

//logs << "START BUY $" << _trailing_stop << "; ";

	}

	if ( ask_ > _trailing_stop ) ///< BUY LONG (ACTION->OPEN)
	{
		
		if ( lock_for_real (id(), is_real ())
		) {
			if ( !real_buy (ask_) )
			{
				logs << error << "REAL BUY request was FAIL" << endl;
				_that_real = false;
				lock_for_real (id(), is_real());
			}
		} else {
			logs << info << " REAL LOCK for BUY denial " << endl;
			 _that_real = false;
		}

		_open_rate = ask_;
		_trail = 9;
		_trailing_stop = bid_ - (float) _trail / multi::factor();
		_target = bid_ + 10.0 / multi::factor();

logs << "BUY at $" << _open_rate << " TARGET=" << _target << " ";

		_is_open = true;
	} 

	_bid = /* new */ bid_; 
	_ask = /* new */ ask_ ; 
}

}; //.buy


///\class sell
///\brief Order for sell

struct sell
	: public order
{
	using order::_trail;
	using order::_bid;
	using order::_ask;
public:

///\brief Create
sell (
  	  const float rate_ ///\param trail_ Current rate
	, const int32_t trail_ = 3 ///\param trail_ Trailing start
	, const bool try_for_real_ = false ///\param try_for_real_ Try it
)
	: order (rate_, trail_, try_for_real_)
{
	_ask = bid() + 100.0 / multi::factor();
	_trailing_stop = bid() - (float) _trail / multi::factor();

logs << " CREATE ORDER SELL ID=" << id() << " START=" << _trailing_stop << " ";
}

///\brief Destroy
virtual ~sell () {
//logs << " DELETE ORDER SELL ";
}

///\brief Set trailing for SELL
virtual
void trail (
	  const int32_t trail_ ///\param trail_ New trailing 
	, float new_rate = 0.0 ///\param new_rate Rate for calculate
) {
	if ( !is_action() && !is_open() ) return;
	_trail = trail_;
	float trailstop = (is_action ()) ? _trail : -_trail;

	if ( 0.0 == new_rate) new_rate = ( is_open() ) ? ask() : bid();
	float new_stop = new_rate + trailstop  / multi::factor();

	if ( is_open ())
	{
		if (new_stop < _trailing_stop) { _trailing_stop = new_stop;

//logs << "TRAIL OPEN STOP=" << _trailing_stop << " ";
		}
	}
	else // is_action ()
		if ( new_stop > _trailing_stop) { _trailing_stop = new_stop;

//logs << "TRAIL ACTION STOP=" << _trailing_stop << " ";
			}
}

///\brief Check out current rate for SELL
virtual
void check (
	  float bid_ ///\param New Bid
	, float ask_ ///\param New Ask
) {
	if ( is_closed () || !is_action() ) return;

	if ( is_open ())
	{
/*******
		if (ask_ < _target)
		{
			_target = ask_ - 10.0 / multi::factor();
			trail (1, ask_);
		}
*******/
		if ( ask_ > _trailing_stop  ///< CLOSE SHORT, CHECK PROFIT
		||  (ask_ < _target)
		)  {
			if ( is_real() )
			{
				if ( !real_buy (ask_))
					throw runtime_error ("Real BUY was FAIL");

				lock_for_real ( id(), false);
			}

			delta_rate pf (_ask, 0);
			pf.rehash (_open_rate);
			_profit = pf.delta;

logs << "CLOSE SHORT SELL at $" << _trailing_stop << "; PROFIT=" << _profit << "; ";
			_is_closed = true;
			_is_open = false;

			return;
		}

		if ( /* new */ ask_ < _ask ) ///< PUSH DOWN TRAILING STOP
		{
			float new_stop = ask_ + (float) _trail / multi::factor();
			if ( new_stop < _trailing_stop) _trailing_stop = new_stop;

/*****
			///< FIX $1
			new_stop = _open_rate - 2.0 / multi::factor();
			if (ask_ < new_stop
			&& _trailing_stop > new_stop )  _trailing_stop = new_stop; 
******/

//logs << "STOP for SELL $" << _trailing_stop << " NEW STOP=" << new_stop << " ";

		}

		_bid = /* new */ bid_; 
		_ask = /* new */ ask_ ; 
		
		return;
	}

	//--IS_ACTION

	if ( bid_ > _bid ) ///< POP UP TRAILING START for SELL
	{
		float new_start = bid_ - (float) _trail / multi::factor();
		if ( new_start > _trailing_stop) { _trailing_stop = new_start; }

//logs << "start for SELL $" << _trailing_stop << "; ";

	}

	if ( bid_ < _trailing_stop ) ///< SELL SHORT (ACTION->OPEN)
	{
		if ( lock_for_real (id(), is_real ())
		) {
			if ( !real_sell (bid_))
			{
				logs << error << "REAL SELL request was FAIL" << endl;
				_that_real = false;
				lock_for_real (id(), is_real());
			}
		} else {
		 	_that_real = false;
			logs << info << " REAL LOCK for SELL was denial " << endl;
		}

		_open_rate = bid_;
		_trail = 9;
		_trailing_stop = ask_ + (float) _trail / multi::factor();
		_target = ask_ - 10.0 / multi::factor();

logs << "SELL at $" << _open_rate << " TARGET=" << _target << "; ";

		_is_open = true;
		return;
	} 

	_bid = /* new */ bid_; 
	_ask = /* new */ ask_ ; 
}

}; //.sell


///\class idle
///\brief Order for idleness

struct idle
	: public order
{
public:

///\brief Create
idle (
	  const float bid_ ///\param bid_ Bid
	, const int32_t trail_ = -1 ///\param trail_ Trailing start
	, const bool try_for_real_ = false ///\param try_for_real_ Never mind
)
	: order (bid_, -1, false)
{}

///\brief Destroy
virtual ~idle () {}

///\brief
virtual
void trail (
	  const int32_t trail_ = -1 ///\param New trailing 
	, float new_rate = 0.0 ///\param new_rate Rate for calculate
)
 {}

///\brief 
virtual
void check (
	  float bid_ ///\param New Bid
	, float ask_ ///\param New Ask
) {
	_bid = bid_;
	_ask = ask_;
}

}; //.idle


} //::deal


#endif //!_DEAL_ORDER_H
