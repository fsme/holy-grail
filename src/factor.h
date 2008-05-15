//@(#) Get multiplies factor by CCY (header)
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _FX_FACTOR_H
#define _FX_FACTOR_H 1

#include <sys/types.h> 
#include <string> 

using namespace std;

namespace fx {


///\class multi
///\brief Multipier factor's singleton
class multi
{
public:

///\brief Get multiplier factor of CCY pair
///\return The factor
static
	int32_t factor ( const std::string& ccy_="" ///\param ccy_ The CCY
) {
	static int32_t _factor = get_factor ( ccy_.c_str());
	return _factor;
}

private:

///\brief Get multiplier factor of CCY pair
///\return The factor
static
	int32_t get_factor ( const char* CCY ///\param CCY of the pair
);

};

///\class lot
///\brief Size of the one lot
class lot
{
public:

///\brief Get money per lot
///\return Size of the lot
static
	int32_t size ( const std::string& ccy_="" ///\param ccy_ The CCY
) {
	static int32_t _size = size_init ( ccy_.c_str());
	return _size;
}

private:

///\brief Initialization size of lot
///\return The size
static
	int32_t size_init ( const char* CCY ///\param CCY of pair
);

};

} //::fx

#endif //!_FX_FACTOR_H
