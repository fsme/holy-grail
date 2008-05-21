//@(#) Order for deal (header)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _DEAL_ORDER_H
#define _DEAL_ORDER_H 1

#include <stdexcept>
#include <cmath>

#include <logostream.hpp>

#include <factor.h>
#include <delta_rate.h>
#include <stat_fifo.h>
#include <request.h>

using namespace std;
using namespace cxx;
using namespace fx;
using namespace stat;

namespace deal {

///\class order
///\brief Virtual order

class order
{
public:

///\brief Create
order (
	  const int32_t trail_///\param trail_ Trailing start
	, const bool try_for_real_///\param try_for_real_ Maybe real order, try it
)
	: _open_rate (0.0)
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
	if ( is_real() ) unlock_for_real ();
}

///\brief Get ID
///\return Identifier of this
int32_t id () const { return _this_id;  }

///\brief Get current profit
///\return Current profit
int32_t profit () const { return _profit;  }

///\brief Get trailing stop
///\return Trailing stop
float trail_stop () const { return _trailing_stop;  }

///\brief Get rate was open
///\return Ask for short, Bid for long
float open_rate () const { return _open_rate;  }

///\brief Get current target
///\return Target for deal
float target () const { return _target;  }

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

///\brief Long or short position
///\return True if long (buy)
bool is_long () { return _that_buy; }

///\brief Close position
virtual
void close () = 0;

///\brief Set trailing for sell
virtual
void trail (
	  const int32_t trail_ ///\param New trailing 
) = 0;

///\brief Check out current rate
virtual
void check () = 0;

protected:
	float	_open_rate; ///< Rate save open position
	float	_trailing_stop; ///< Rate for trailing
	float	_target;	///< Rate for trailing

	int32_t	_trail;		///< Trailing in pips
	int32_t _profit;	///< At pipies
	int32_t	_this_id;	///< Identifier of order, random

	bool	_is_open;	///< In the deal
	bool	_is_closed;	///< Outside
	bool	_that_real;	///< Real live
	bool	_that_buy; ///< That long

///\brief Try lock for real deal
///\return True if locked; False if unlocked
bool lock_for_real ( const bool lock_ ///\param lock_ Lock if true unlock else
);

///\brief Unlock if real locked
void unlock_for_real () { lock_for_real (false); }

///\brief Real request for BUY
///\return False if fail
bool
	real_buy ()
{
	if ( env::iron.exists ("r")) return re::que::st().buy();
	//else
		return re::que::st().echo("BUY");
}

///\brief Real request for SELL
///\return False if fail
bool
	real_sell ()
{
	if ( env::iron.exists ("r")) return re::que::st().sell ();
	//else
		return re::que::st().echo("SELL");
}

}; //.order


///\class buy
///\brief Order for BUY

struct buy
	: public order
{
	using order::_trail;
public:

///\brief Create
buy (
	  const int32_t trail_= 3 ///\param trail_ Trailing start
	, const bool try_for_real_ = false ///\param try_for_real_ Try it
)
	: order (trail_, try_for_real_)
{
	_that_buy = true;
	_trailing_stop = last_ask() + (float) _trail / multi::factor();

logs << " CREATE ORDER BUY ID=" << id() << " START=" << trail_stop() << " ";
}

///\brief Destroy
virtual ~buy () {
logs << " DELETE ORDER BUY ";
}

///\brief Set new trailing for BUY
virtual
void trail (
	  const int32_t trail_ ///\param New trailing 
) {
	if ( !is_action() && !is_open() ) return;
	_trail = trail_;
	float trailstop = (is_action ()) ? -_trail : _trail;

	float new_rate = ( is_open() ) ? last_bid() : last_ask();
	float new_stop = new_rate + trailstop  / multi::factor();

	if ( is_open ())
	{
		if ( new_stop > trail_stop() ) { _trailing_stop = new_stop;

logs << "TRAIL for OPEN STOP=" << trail_stop() << " ";

		}
	}
	else // is_action ()
		if ( new_stop < trail_stop() ) { _trailing_stop = new_stop;

logs << "TRAIL ACTION STOP=" << trail_stop() << " ";
			}
}

///\brief Close position which BUY
virtual
	void close ()
{
	if ( is_open())
	{
		if ( is_real())
		{
			if ( !real_sell () )
				throw runtime_error ("Real SELL was FAIL");

			unlock_for_real ();
		}

		delta_rate pf (_open_rate, 0);
		pf.rehash ( last_bid());
		_profit = pf.delta;

logs << "CLOSE LONG BUY at $" << last_bid() << "; PROFIT=" << _profit << "; ";

		_is_closed = true;
		_is_open = false;
	}

	_trail = -1;
}

///\brief Check out current rate for BUY
virtual
void check ()
{
	if ( is_closed () || !is_action() ) return;

	if ( is_open ())
	{
		if ( last_bid() > target() ) ///< ZERO size TRAILing stop
		{
			_target = last_bid() + 100.0 / multi::factor();
			trail (0);
		}

		if ( last_bid() < trail_stop()  ///< CLOSE LONG
		||  (last_bid() > target() )
		) {
			close ();
			return;
		}

		if ( last_bid() > prev_bid() ) ///< POP UP TRAILING STOP
		{
			float new_stop = last_bid() - (float) _trail / multi::factor();
			if ( new_stop > trail_stop() ) { _trailing_stop = new_stop; }

logs << "STOP for BUY $" << trail_stop() << " NEW STOP=" << new_stop << " ";
		}

		return;
	}

	//--IS_ACTION

	if ( last_ask() < prev_ask() ) ///< PUSH DOWN TRAILING START
	{
		float new_start = last_ask() + (float) _trail / multi::factor();
		if ( new_start < trail_stop() ) { _trailing_stop = new_start; }

//logs << "START BUY $" << trail_stop() << "; ";

	}

	if ( last_ask() > trail_stop() ) ///< BUY LONG (ACTION->OPEN)
	{
		
		if ( lock_for_real ( is_real ())
		) {
			if ( !real_buy () )
			{
				logs << error << "REAL Buy request fail" << endl;
				_that_real = false;
				unlock_for_real ();
			}
		} else {
			logs << info << "Lock for buy was denail" << endl;
			 _that_real = false;
		}

		_open_rate = last_ask();
		_trail = 41;
		_trailing_stop = last_bid() - (float) _trail / multi::factor();
		_target = last_bid() + 10.0 / multi::factor();
		_is_open = true;

logs << "BUY at $" << _open_rate << " TARGET=" << target() << " ";

		return;
	} 
}

}; //.buy


///\class sell
///\brief Order for sell

struct sell
	: public order
{
	using order::_trail;
public:

///\brief Create
sell (
	  const int32_t trail_ = 3 ///\param trail_ Trailing start
	, const bool try_for_real_ = false ///\param try_for_real_ Try it
)
	: order (trail_, try_for_real_)
{
	_that_buy = false;
	_trailing_stop = last_bid() - (float) _trail / multi::factor();

logs << " CREATE ORDER SELL ID=" << id() << " START=" << trail_stop() << " ";
}

///\brief Destroy
virtual ~sell () {
logs << " DELETE ORDER SELL ";
}

///\brief Set trailing for SELL
virtual
void trail (
	  const int32_t trail_ ///\param trail_ New trailing 
) {
	if ( !is_action() && !is_open() ) return;
	_trail = trail_;
	float trailstop = (is_action ()) ? _trail : -_trail;

	float new_rate = ( is_open() ) ? last_ask() : last_bid();
	float new_stop = new_rate + trailstop  / multi::factor();

	if ( is_open ())
	{
		if (new_stop < trail_stop() ) { _trailing_stop = new_stop;

//logs << "TRAIL OPEN STOP=" << trail_stop() << " ";
		}
	}
	else // is_action ()
		if ( new_stop > trail_stop() ) { _trailing_stop = new_stop;

//logs << "TRAIL ACTION STOP=" << trail_stop() << " ";
			}
}

///\brief Close SELL
virtual
	void close ()
{
	if ( is_open ())
	{
		if ( is_real() )
		{
			if ( !real_buy () )
				throw runtime_error ("Real BUY was FAIL");

			unlock_for_real ();
		}

		delta_rate pf ( last_ask(), 0);
		pf.rehash (_open_rate);
		_profit = pf.delta;

logs << "CLOSE SHORT SELL at $" << last_ask() << "; PROFIT=" << profit() << "; ";
		_is_closed = true;
		_is_open = false;
	}

	_trail = -1;
}

///\brief Check out current rate for SELL
virtual
void check ()
{
	if ( is_closed () || !is_action() ) return;

	if ( is_open ())
	{
		if ( last_ask() < target() ) ///< ZERO size TRAILing stop
		{
			_target = last_ask() - 100.0 / multi::factor();
			trail (0);
		}

		if ( last_ask() > trail_stop()  ///< CLOSE SHORT, CHECK PROFIT
		||  (last_ask() < target() )
		)  {
			close();
			return;
		}

		if ( last_ask() < prev_ask() ) ///< PUSH DOWN TRAILING STOP
		{
			float new_stop = last_ask() + (float) _trail / multi::factor();
			if ( new_stop < trail_stop() ) _trailing_stop = new_stop;

logs << "STOP for SELL $" << trail_stop() << " NEW STOP=" << new_stop << " ";
		}

		return;
	}

	//--IS_ACTION

	if ( last_bid() > prev_bid() ) ///< POP UP TRAILING START for SELL
	{
		float new_start = last_bid() - (float) _trail / multi::factor();
		if ( new_start > trail_stop() ) { _trailing_stop = new_start; }

logs << "start for SELL $" << trail_stop() << "; ";

	}

	if ( last_bid() < trail_stop() ) ///< SELL SHORT (ACTION->OPEN)
	{
		if ( lock_for_real (is_real())
		) {
			if ( !real_sell ())
			{
				logs << error << "Real SELL request fail" << endl;
				_that_real = false;
				unlock_for_real ();
			}
		} else {
		 	_that_real = false;
			logs << info << "Lock for SELL was denial" << endl;
		}

		_open_rate = last_bid();
		_trail = 41;
		_trailing_stop = last_ask() + (float) _trail / multi::factor();
		_target = last_ask() - 10.0 / multi::factor();
		_is_open = true;

logs << "SELL at $" << open_rate() << " TARGET=" << target() << "; ";
		return;
	} 
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
	  const int32_t trail_ = -1 ///\param trail_ Trailing start
	, const bool try_for_real_ = false ///\param try_for_real_ Never mind
)
	: order (-1, false)
{}

///\brief Destroy
virtual ~idle () {}

///\brief Close virtual, never mind
virtual
void trail (
	  const int32_t trail_ = -1 ///\param New trailing 
) {}

///\brief Close virtual, never mind
virtual void close () {}

///\brief Close virtual, never mind
virtual void check () {}

}; //.idle


} //::deal


#endif //!_DEAL_ORDER_H
