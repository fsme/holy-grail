//@(#) Simple sum and simple average template
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id$

#ifndef _AVE_RAGE_HPP
#define _AVE_RAGE_HPP 1

#include <logostream.hpp>

#include <fifo.hpp>

using namespace std;
using namespace cxx;

namespace ave {

///\class fifo_avrg
///\brief Sum and simple avarage of the base FIFO

template <typename Type>
class fifo_avrg
	: public fi::fo<Type>
{
typedef size_t			size_type;
typedef fi::fo<Type>	base_type;

public:

///\brief Create
fifo_avrg (
	  const size_t max_ ///\param max_ Maximum size of FIFO
)
	: fi::fo<Type> (max_)
	, _sum  (0)
	, _avrg (0.0)
{}

///\brief Destroy
virtual ~fifo_avrg () {}

///\brief
virtual
void push_back (
		const Type& val_ ///\param val_
) {
	if ( base_type::is_full () ) base_type::pop_front();
	base_type::push_back (val_);

	_sum = (Type)0;
	for (size_type i = 0; i < base_type::size(); ++i)
		_sum += base_type::operator[] (i);

	_avrg = (float)_sum / (float)base_type::size();
}

///\brief Get sum
///\return Average
const Type sum () const { return ( base_type::is_full() ) ? _sum : (Type)0; }

///\brief Get average
///\return Simple average
float avrg () const { return  (base_type::is_full() ) ? _avrg : 0.0; }

private:
	Type	_sum; ///\ Sum
	float	_avrg; ///\ Average
};


///\class basic_average
///\brief Virtual sum and simple avarage

template <typename Type>
class basic_average
{
public:
typedef size_t			size_type;

///\brief Create
basic_average (
	  const size_t size_ ///\param max_ Size for calculation
)
	: _size (size_)
	, _sum  (0)
	, _avrg (0.0)
{}

///\brief Create
virtual ~basic_average () {}

///\brief Virtual rehash
virtual
void rehash (
		const fi::fo<Type>& fifo_ ///\param fifo_ For computing
) = 0;

///\brief Get sum
///\return Sum
	const Type sum () const { return _sum; }

///\brief Get average
///\return Average
	float avrg () const { return _avrg; }

///\brief Get size
///\return Size
	size_type size () const { return _size; }

///\brief Cleared for action
///\return True if is it
virtual
	bool is_ready () const = 0;

protected:
	Type		_sum; ///\ Sum
	float		_avrg; ///\ Simple average

private:
	size_type	_size;
};

///\class basic_direct
///\brief Direct of FIFO

template <typename Type>
class basic_direct
{
public:
typedef size_t			size_type;

///\brief Create
basic_direct (
	  const size_t size_ ///\param max_ Size for calculation
)
	: _size (size_)
	, _dir (0)
{}

///\brief Create
virtual ~basic_direct () {}

///\brief Virtual recalc
virtual
void recalc (
		const fi::fo<Type>& fifo_ ///\param fifo_ For computing
) = 0;

///\brief Get direct
///\return The direct: Less then, Zero or More then zero
	int32_t direct () const { return _dir; }

///\brief Get size
///\return Size
	size_type size () const { return _size; }

protected:
	int32_t 	_dir; ///< Direct

private:
	size_type	_size;
};


} //::ave

#endif //!_AVE_RAGE_HPP
