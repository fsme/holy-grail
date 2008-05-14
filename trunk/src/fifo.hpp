//@(#) FIFO with size is limited
// Author: Andrew Wingorodov <http://andr.mobi/>
// Licence: GPLv3
// $Id: fifo.hpp 27 2008-05-11 11:45:45Z wingorodov $

#ifndef _FI_FO_H
#define _FI_FO_H 1

#include <deque>
#include <stdexcept>

#include <logostream.hpp>

using namespace std;
using namespace cxx;

namespace fi {

///\class fo
///\brief Virtual basic FIFO

template <typename T>
class basic_fo
{
public:
typedef size_t	size_type;

///\brief Create
basic_fo ( const size_type& max_ ///\param max_ Maximum size
)
	: _max (max_)
{}

virtual ~basic_fo () {}

///\name Push and pop front and back
//@{
///\brief Push front is empty at FIFO
void push_front ( const T& val_ ///\param val_ 
)
 { /* never mind */ }

///\brief Push back
virtual
void push_back ( const T& val_ ///\param val_ 
) = 0;

///\brief Pop front
virtual
void pop_front () = 0;

///\brief Pop back is empty at FIFO
void pop_back ()
	{ /* never mind */ }

///\brief Get back
///\brief Back element
virtual
const T& back () const = 0;

///\brief Get front
///\brief Front element
virtual
const T& front () const = 0;

//@}

///\brief Get element by index
///\return Reference to element 
T& operator[] ( const size_type index_ ///\param index_ 
) {
	if (index_ < 0 || index_ >= queue.size())
		throw out_of_range ("fifo::operator[]: out of range");

	return queue [index_];
}

///\brief Get const element by index
///\return Reference to element 
const T& operator[] ( const size_type index_ ///\param index_ 
) const {
	if (index_ < 0 || index_ >= queue.size())
		throw out_of_range ("fifo::operator[]: out of range");

	return queue [index_];
}

///\brief Is full
///\brief True if queue is full
bool
	is_full () const { return max_size() == size(); }

///\brief Cleared for action
virtual
	bool ready () const = 0;

///\brief Get size
///\return Current queue size
size_type
	size () const { return queue.size(); }

///\brief Set new maximum size
///\return Maximum size
size_type max_size (
	  const size_type max_ ///\param max_ New limit for size
) {
	if ( size() > max_)
		while ( size() !=  max_) queue.pop_front();

	return _max = max_;
}

///\brief Get maximum size
///\return Current maximum size
size_type
	max_size () const { return _max; }

///\brief Show queue
virtual
void print (
	  const std::string& pre_ = "" ///\param pre_ Prefix before show
	, const std::string& col_ = " " ///\param col_ Colon for div
) const = 0;

///\brief Unit for test
virtual
bool unitst () = 0;


protected:
	std::deque<T>	queue;

private:
	size_type		_max;

}; //.class basic_fo



///\class fo
///\brief Double-ended queue with limited size

template <typename T>
class fo
	: public basic_fo<T>
{
public:
typedef basic_fo<T>				base_type;
typedef size_t					size_type;

///\brief Create
fo ( const size_type& max_ = 0  ///\param max_ Maximum size
)
	: basic_fo<T> (max_)
{}

virtual ~fo () {}

///\brief Push and size control
virtual
void push_back ( const T& val_ ///\param val_ 
) {
	if ( base_type::size() == base_type::max_size()) pop_front ();
	base_type::queue.push_back (val_);
}

///\brief Pop from from 
virtual
void pop_front () { base_type::queue.pop_front (); }

///\brief Get back
///\return Reference to back element of base queue
virtual
const T& back () const { return base_type::queue.back (); } 

///\brief Get front
///\return Reference to front element of base queue
virtual
const T& front () const { return base_type::queue.front (); } 

///\brief Cleared for action
///\return True if it is cleared for action
virtual
	bool ready () const { return base_type::is_full (); }

///\brief Show queue
virtual
void print (
	  const std::string& pre_ = "" ///\param pre_ Prefix before show
	, const std::string& col_ = " " ///\param col_ Colon
) const {
	if ( base_type::size() <= 0	) return;

	logs << notice << pre_ ;
	for (size_type i = 0; i < base_type::size(); ++i)
		logs << notice << "" << base_type::queue [i] << col_ << flush;
}

///\brief Unit for test
virtual
	bool unitst ()
{
	return  true;
}

}; //.class fo

} //::fi


#endif //_FI_FO_H 1
