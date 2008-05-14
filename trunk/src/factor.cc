//@(#) Get factor by CCY (code)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id: factor.cc 22 2008-05-07 19:28:41Z wingorodov $

#include <sys/types.h> 

#include <factor.h> 

namespace fx {

struct rates_factor
{
	const char*		ccy;
	int32_t			mult;

} rates_factor_list [] = {

	  { "AUD/CAD", 10000 }
	, { "AUD/CHF", 10000 }
	, { "AUD/JPY", 100 }
	, { "AUD/NZD", 10000 }
	, { "AUD/USD", 10000 }

	, { "CAD/CHF", 10000 }
	, { "CAD/JPY", 100 }

	, { "CHF/JPY", 100 }

	, { "EUR/AUD", 10000 }
	, { "EUR/CAD", 10000 }
	, { "EUR/CHF", 10000 }
	, { "EUR/GBP", 10000 }
	, { "EUR/JPY", 100 }
	, { "EUR/NZD", 10000 }
	, { "EUR/USD", 10000 }

	, { "GBP/AUD", 10000 }
	, { "GBP/CAD", 10000 }
	, { "GBP/CHF", 10000 }
	, { "GBP/JPY", 100 }
	, { "GBP/NZD", 10000 }
	, { "GBP/USD", 10000 }

	, { "NZD/CAD", 10000 }
	, { "NZD/CHF", 10000 }
	, { "NZD/JPY", 100 }
	, { "NZD/USD", 10000 }

	, { "USD/CAD", 10000 }
	, { "USD/CHF", 10000 }
	, { "USD/JPY", 100 }

};

//
int32_t multi::get_factor (const char* CCY)
{
	static const
	size_t max = sizeof ( rates_factor_list ) / sizeof ( struct rates_factor );

	size_t num = 0;
	for (; num < max; ++num)
	{
		if ( 0 == ::strncmp ( rates_factor_list [num].ccy, CCY, 7) )
			return rates_factor_list [num].mult;
	}
	return 0;
}

//
int32_t lot::size_init (const char* CCY)
{
	return 10000;
}

} //::fx
